/*
 * Copyright 2025 bgewehr (bg-hems branch)
 * Licensed under the Apache License, Version 2.0
 */

#include "eebus_wp.h"

#include <cmath>
#include <cstring>

#include "esphome/core/log.h"

#include <nvs_flash.h>
#include <nvs.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>

extern "C" {
#include "src/service/service/eebus_service.h"
#include "src/service/service/eebus_service_config.h"
#include "src/common/eebus_device_info.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "src/use_case/model/load_limit_types.h"
}

#include "port/esp32/websocket/websocket_client_esp32.h"
#include "port/esp32/websocket/websocket_server_esp32.h"

namespace esphome {
namespace eebus_wp {

/* NVS keys — separate namespace from eebus_lpc to avoid conflicts */
static const char* NVS_NS       = "eebus_wp";
static const char* NVS_KEY_CERT = "cert_der";
static const char* NVS_KEY_KEY  = "key_der";

/* =========================================================================
 * setup()
 * ====================================================================== */

void EebusWpComponent::setup() {
  ESP_LOGI(TAG, "Setting up EEBus WP controller (K40RF)");

  uint8_t* cert = nullptr; size_t cl = 0;
  uint8_t* key  = nullptr; size_t kl = 0;

  if (!load_cert_nvs_(&cert, &cl, &key, &kl)) {
    ESP_LOGI(TAG, "Generating certificate...");
    if (!load_or_generate_cert_() || !load_cert_nvs_(&cert, &cl, &key, &kl)) {
      ESP_LOGE(TAG, "Certificate setup failed");
      mark_failed(); return;
    }
  }

  if (!start_eebus_service_(cert, cl, key, kl)) {
    ESP_LOGE(TAG, "Failed to start openeebus service");
    free(cert); free(key);
    mark_failed(); return;
  }

  free(cert); free(key);
  ESP_LOGI(TAG, "EEBus WP ready — searching for K40RF via mDNS...");
}

/* =========================================================================
 * loop()
 * ====================================================================== */

void EebusWpComponent::loop() {
  /* Nothing to poll — openeebus runs on FreeRTOS tasks */
}

/* =========================================================================
 * dump_config()
 * ====================================================================== */

void EebusWpComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "EEBus WP Component (Bosch K40RF):");
  ESP_LOGCONFIG(TAG, "  SHIP Port:      %d",   ship_port_);
  ESP_LOGCONFIG(TAG, "  K40RF SKI:      %s",   remote_ski_.empty() ? "(auto-discover)" : remote_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Connected:      %s",   connected_ ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Active limit:   %.0f W", active_limit_w_);
  ESP_LOGCONFIG(TAG, "  Failsafe:       %.0f W / %u s", failsafe_limit_w_, failsafe_duration_s_);
}

/* =========================================================================
 * Public API
 * ====================================================================== */

void EebusWpComponent::set_limit(float watts) {
  if (!connected_ || !have_remote_entity_ || !eg_lpc_) {
    ESP_LOGW(TAG, "set_limit(%.0f W) — K40RF not connected, ignoring", watts);
    return;
  }

  /* §14a: enforce minimum 4200 W — never send below unless 0 (clear) */
  if (watts > 0.0f && watts < 4200.0f) {
    ESP_LOGW(TAG, "Requested limit %.0f W < 4200 W minimum, clamping to 4200 W", watts);
    watts = 4200.0f;
  }

  LoadLimit limit;
  memset(&limit, 0, sizeof(limit));

  if (watts <= 0.0f) {
    /* Clear limit — send maximum (no restriction) */
    limit.value.value = 99999;
    limit.value.scale = 0;
    limit.is_active   = false;
    ESP_LOGI(TAG, "Clearing WP power limit");
  } else {
    /* Convert watts to ScaledValue (integer, scale 0 = direct watts) */
    limit.value.value = (int64_t)watts;
    limit.value.scale = 0;
    limit.is_active   = true;
    ESP_LOGI(TAG, "Setting WP power limit: %.0f W", watts);
  }

  EebusError err = EgLpcSetActiveConsumptionPowerLimit(eg_lpc_, &remote_entity_addr_, &limit);
  if (err != kEebusErrorOk) {
    ESP_LOGE(TAG, "EgLpcSetActiveConsumptionPowerLimit failed: %d", (int)err);
    return;
  }

  active_limit_w_ = watts > 0.0f ? watts : 0.0f;
}

/* =========================================================================
 * EgLpListenerInterface callbacks
 * ====================================================================== */

void EebusWpComponent::on_entity_connect(const EntityAddressType* addr) {
  ESP_LOGI(TAG, "K40RF entity connected");
  connected_       = true;
  have_remote_entity_ = true;
  remote_entity_addr_ = *addr;
  pairing_state_   = "Verbunden mit K40RF";

  /* Send failsafe configuration on connect */
  ScaledValue fs_limit;
  fs_limit.value = (int64_t)failsafe_limit_w_;
  fs_limit.scale = 0;
  EgLpcSetFailsafeConsumptionActivePowerLimit(eg_lpc_, addr, &fs_limit);

  EebusDuration fs_duration;
  fs_duration.seconds = failsafe_duration_s_;
  EgLpcSetFailsafeDurationMinimum(eg_lpc_, addr, &fs_duration);

  /* Start heartbeat so K40RF knows we're alive */
  EgLpcStartHeartbeat(eg_lpc_);

  for (auto* t : connected_triggers_) t->trigger();
}

void EebusWpComponent::on_entity_disconnect(const EntityAddressType* /*addr*/) {
  ESP_LOGW(TAG, "K40RF entity disconnected");
  connected_          = false;
  have_remote_entity_ = false;
  active_limit_w_     = 0.0f;
  pairing_state_      = "Getrennt — suche K40RF...";

  EgLpcStopHeartbeat(eg_lpc_);
  for (auto* t : disconnected_triggers_) t->trigger();
}

void EebusWpComponent::on_power_limit_ack(float watts, bool active) {
  /* K40RF confirms it received and applied the limit */
  ESP_LOGD(TAG, "K40RF ACK limit %.0f W active=%s", watts, active ? "yes" : "no");
}

void EebusWpComponent::on_power_reading(float watts) {
  current_power_w_ = watts;
  for (auto* t : power_triggers_) t->trigger(watts);
}

/* =========================================================================
 * NVS certificate helpers
 * ====================================================================== */

bool EebusWpComponent::store_cert_nvs_(
    const uint8_t* c, size_t cl, const uint8_t* k, size_t kl)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return false;
  bool ok = (nvs_set_blob(h, NVS_KEY_CERT, c, cl) == ESP_OK) &&
            (nvs_set_blob(h, NVS_KEY_KEY,  k, kl) == ESP_OK) &&
            (nvs_commit(h) == ESP_OK);
  nvs_close(h);
  return ok;
}

bool EebusWpComponent::load_cert_nvs_(
    uint8_t** c, size_t* cl, uint8_t** k, size_t* kl)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return false;
  size_t clen = 0, klen = 0;
  bool ok = false;
  do {
    if (nvs_get_blob(h, NVS_KEY_CERT, nullptr, &clen) != ESP_OK || clen == 0) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  nullptr, &klen) != ESP_OK || klen == 0) break;
    *c = (uint8_t*)malloc(clen); *k = (uint8_t*)malloc(klen);
    if (!*c || !*k) { free(*c); free(*k); break; }
    if (nvs_get_blob(h, NVS_KEY_CERT, *c, &clen) != ESP_OK) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  *k, &klen) != ESP_OK) break;
    *cl = clen; *kl = klen; ok = true;
  } while (false);
  nvs_close(h);
  if (!ok) { free(*c); free(*k); *c = *k = nullptr; }
  return ok;
}

bool EebusWpComponent::load_or_generate_cert_() {
  mbedtls_pk_context       pk;
  mbedtls_x509write_cert   crt;
  mbedtls_entropy_context  entropy;
  mbedtls_ctr_drbg_context drbg;
  mbedtls_mpi              serial;

  mbedtls_pk_init(&pk);
  mbedtls_x509write_crt_init(&crt);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_mpi_init(&serial);

  bool ok = false;
  do {
    const char* pers = "eebus_wp_eg";
    if (mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char*)pers, strlen(pers)) != 0) break;
    if (mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0 ||
        mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(pk),
                            mbedtls_ctr_drbg_random, &drbg) != 0) break;

    uint8_t key_buf[2048]; memset(key_buf, 0, sizeof(key_buf));
    int key_len = mbedtls_pk_write_key_der(&pk, key_buf, sizeof(key_buf));
    if (key_len <= 0) break;
    uint8_t* key_p = key_buf + sizeof(key_buf) - key_len;

    std::string subj = "CN=" + device_model_ + "-WP-EG,O=" + device_brand_ + ",C=DE";
    mbedtls_x509write_crt_set_subject_key(&crt, &pk);
    mbedtls_x509write_crt_set_issuer_key(&crt, &pk);
    mbedtls_x509write_crt_set_subject_name(&crt, subj.c_str());
    mbedtls_x509write_crt_set_issuer_name(&crt, subj.c_str());
    mbedtls_x509write_crt_set_version(&crt, MBEDTLS_X509_CRT_VERSION_3);
    mbedtls_x509write_crt_set_md_alg(&crt, MBEDTLS_MD_SHA256);
    mbedtls_mpi_read_string(&serial, 10, "1");
    mbedtls_x509write_crt_set_serial(&crt, &serial);
    mbedtls_x509write_crt_set_validity(&crt, "20250101000000", "20350101000000");
    mbedtls_x509write_crt_set_subject_key_identifier(&crt);
    mbedtls_x509write_crt_set_authority_key_identifier(&crt);

    uint8_t cert_buf[4096];
    int cert_len = mbedtls_x509write_crt_der(&crt, cert_buf, sizeof(cert_buf),
                                              mbedtls_ctr_drbg_random, &drbg);
    if (cert_len <= 0) break;
    uint8_t* cert_p = cert_buf + sizeof(cert_buf) - cert_len;
    ok = store_cert_nvs_(cert_p, (size_t)cert_len, key_p, (size_t)key_len);
  } while (false);

  mbedtls_pk_free(&pk);
  mbedtls_x509write_crt_free(&crt);
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&drbg);
  mbedtls_mpi_free(&serial);
  return ok;
}

/* =========================================================================
 * Start openeebus service as EG (client connecting to K40RF)
 * ====================================================================== */

bool EebusWpComponent::start_eebus_service_(
    const uint8_t* cert_der, size_t cert_len,
    const uint8_t* key_der,  size_t key_len)
{
  TlsCertificateObject* tls_cert = TlsCertificateParseX509KeyPair(
      (const char*)cert_der, cert_len, (const char*)key_der, key_len);
  if (!tls_cert) { ESP_LOGE(TAG, "TlsCertificateParse failed"); return false; }

  const char* local_ski = TLS_CERTIFICATE_GET_SKI(tls_cert);
  ESP_LOGI(TAG, "WP-EG local SKI: %s", local_ski ? local_ski : "(null)");

  /* The K40RF is the SHIP server — we need the client-side WebSocket creator */
  /* Discovery is automatic via mDNS; remote_ski used for filtering if set */
  WebsocketCreatorObject* ws_creator = WebsocketClientCreatorEsp32Create(
      /* URI built dynamically after mDNS discovery; empty for now */
      "wss://placeholder:4712/ship/",
      cert_der, cert_len,
      key_der,  key_len,
      nullptr, 0  /* no CA — TOFU pairing */);

  if (!ws_creator) { ESP_LOGE(TAG, "WS client creator failed"); return false; }

  EebusServiceConfig* cfg = EebusServiceConfigCreate(
      "DIY",
      device_brand_.c_str(),
      device_model_.c_str(),
      "HEMS-WP-01",
      "EnergyManagementSystem",
      ship_port_);
  if (!cfg) { ESP_LOGE(TAG, "EebusServiceConfigCreate failed"); return false; }

  /* Auto-accept: find K40RF via mDNS automatically */
  EebusServiceConfigSetRegisterAutoAccept(cfg, remote_ski_.empty());

  service_ = EebusServiceCreate(cfg);
  EebusServiceConfigDelete(cfg);
  if (!service_) { ESP_LOGE(TAG, "EebusServiceCreate failed"); return false; }

  /* Register K40RF SKI if configured, else auto-accept first found */
  if (!remote_ski_.empty()) {
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, remote_ski_.c_str(), true);
  }

  /* EG listener */
  EG_LP_LISTENER_INTERFACE(&eg_listener_) = &kEgListenerMethods;
  eg_listener_.self = this;

  /* Get local entity from service and attach EG LPC use case */
  DeviceLocalObject* local_device = EEBUS_SERVICE_GET_LOCAL_DEVICE(service_);
  if (!local_device) { ESP_LOGE(TAG, "GetLocalDevice failed"); return false; }

  /* EgLpcUseCaseCreate needs a local entity — use first entity of device */
  /* This is the standard pattern from eebus-go examples */
  eg_lpc_ = EgLpcUseCaseCreate(
      /* entity */ nullptr,   /* service will inject correct entity */
      EG_LP_LISTENER_OBJECT(&eg_listener_));

  if (!eg_lpc_) { ESP_LOGE(TAG, "EgLpcUseCaseCreate failed"); return false; }

  EebusError err = EEBUS_SERVICE_START(service_);
  if (err != kEebusErrorOk) {
    ESP_LOGE(TAG, "EEBUS_SERVICE_START failed: %d", (int)err);
    return false;
  }

  pairing_state_ = "Suche K40RF via mDNS...";
  ESP_LOGI(TAG, "EEBus WP service started, searching for K40RF");
  return true;
}

}  // namespace eebus_wp
}  // namespace esphome
