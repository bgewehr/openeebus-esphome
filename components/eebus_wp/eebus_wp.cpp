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
#include "src/service/api/eebus_service_config.h"
#include "src/service/api/service_reader_interface.h"
#include "src/common/eebus_device_info.h"
#include "src/ship/api/mdns_entry.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "src/use_case/model/load_limit_types.h"
}

#include "port/esp32/websocket/websocket_server_esp32.h"

namespace esphome {
namespace eebus_wp {

static const char* NVS_NS        = "eebus_wp";
static const char* NVS_KEY_CERT  = "cert_der";
static const char* NVS_KEY_KEY   = "key_der";
static const char* NVS_KEY_SKI   = "remote_ski";

/* SPINE spec: EG sends heartbeat every kHeartbeatTimeoutSeconds.
 * WP expects a heartbeat within this window — if missed it raises a fault.
 * 60 s matches the openeebus reference HEMS example. */
static const uint32_t kHeartbeatTimeoutSeconds = 60;

/* =========================================================================
 * ServiceReader C vtable — bridges SHIP pairing events to C++ component
 * ====================================================================== */

struct WpServiceReader {
  ServiceReaderObject obj;   /* must be first */
  EebusWpComponent*   self;
};

extern "C" {

static void SR_Destruct(ServiceReaderObject*) {}

static void SR_OnRemoteSkiConnected(
    ServiceReaderObject* o, EebusServiceObject* /*svc*/, const char* ski)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  ESP_LOGW("eebus_wp", "Remote SKI connected: %s", ski);
  r->self->pairing_state_ = "Verbinde WP: " + std::string(ski);
}

static void SR_OnRemoteSkiDisconnected(
    ServiceReaderObject* o, EebusServiceObject* /*svc*/, const char* ski)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  ESP_LOGW("eebus_wp", "Remote SKI disconnected: %s", ski);
  r->self->on_entity_disconnect(nullptr);
}

static void SR_OnRemoteServicesUpdate(
    ServiceReaderObject* o, EebusServiceObject* svc, const Vector* entries)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  if (!r->self->remote_ski_.empty()) return;  /* already targeting a specific device */

  size_t n = VectorGetSize(entries);
  for (size_t i = 0; i < n; i++) {
    const MdnsEntry* entry = (const MdnsEntry*)VectorGetElement(entries, i);
    const char* ski = MdnsEntryGetSki(entry);
    if (!ski || ski[0] == '\0') continue;
    if (r->self->local_ski_ == ski) continue;  /* skip ourselves */
    ESP_LOGW("eebus_wp", "mDNS: discovered WP ski=%s host=%s, registering",
             ski, MdnsEntryGetHost(entry) ? MdnsEntryGetHost(entry) : "?");
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(svc, ski, true);
    r->self->pairing_state_ = "mDNS: Verbinde " + std::string(ski);
    break;  /* register only the first discovered candidate */
  }
}

static void SR_OnShipIdUpdate(
    ServiceReaderObject*, const char* ski, const char* ship_id)
{
  ESP_LOGD("eebus_wp", "SHIP ID update ski=%s id=%s", ski, ship_id ? ship_id : "");
}

static void SR_OnShipStateUpdate(
    ServiceReaderObject* o, const char* ski, SmeState state)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  ESP_LOGW("eebus_wp", "SHIP state ski=%s state=%d", ski, (int)state);
  if (state == kDataExchange) {
    r->self->remote_ski_ = ski;
    r->self->pairing_state_ = "Gepairt: " + std::string(ski);
    ESP_LOGW("eebus_wp", "WP pairing approved, remote SKI=%s", ski);
    r->self->save_remote_ski_nvs_(ski);
    r->self->on_ship_data_exchange_(ski);
  }
}

static bool SR_IsWaitingForTrustAllowed(const ServiceReaderObject* o, const char* ski) {
  if (!ski || strcmp(ski, "unknown") == 0 || strlen(ski) < 20) return false;
  return reinterpret_cast<const WpServiceReader*>(o)->self->pairing_mode_;
}

static const ServiceReaderInterface kServiceReaderMethods = {
  .destruct                   = SR_Destruct,
  .on_remote_ski_connected    = SR_OnRemoteSkiConnected,
  .on_remote_ski_disconnected = SR_OnRemoteSkiDisconnected,
  .on_remote_services_update  = SR_OnRemoteServicesUpdate,
  .on_ship_id_update          = SR_OnShipIdUpdate,
  .on_ship_state_update       = SR_OnShipStateUpdate,
  .is_waiting_for_trust_allowed = SR_IsWaitingForTrustAllowed,
};

}  // extern "C"

/* =========================================================================
 * setup()
 * ====================================================================== */

void EebusWpComponent::setup() {
  ESP_LOGI(TAG, "Setting up EEBus WP controller");

  /* Load paired K40RF SKI from NVS if not set in YAML secrets */
  if (remote_ski_.empty()) {
    remote_ski_ = load_remote_ski_nvs_();
    if (!remote_ski_.empty()) {
      if (remote_ski_ == "unknown" || remote_ski_.length() < 20) {
        ESP_LOGW(TAG, "Clearing invalid SKI from NVS: '%s'", remote_ski_.c_str());
        save_remote_ski_nvs_("");
        remote_ski_.clear();
      } else {
        ESP_LOGW(TAG, "Loaded paired WP SKI from NVS: %s", remote_ski_.c_str());
      }
    }
  }

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
  ESP_LOGI(TAG, "EEBus WP ready — heartbeat interval: %u s", kHeartbeatTimeoutSeconds);
}

/* =========================================================================
 * loop()
 * ====================================================================== */

void EebusWpComponent::loop() {
  if (!service_ || !eg_lpc_) return;

  /* Verify the heartbeat is being acknowledged by WP.
   * EgLpcIsHeartbeatWithinDuration() returns false if no heartbeat ACK
   * was received within 2× kHeartbeatTimeoutSeconds.
   * In that case the connection is likely stale — log and flag. */
  if (connected_ && !EgLpcIsHeartbeatWithinDuration(eg_lpc_)) {
    if (!heartbeat_alarm_) {
      ESP_LOGW(TAG, "WP heartbeat overdue \xe2\x80\x94 connection may be stale");
      heartbeat_alarm_ = true;
      pairing_state_ = "Heartbeat-Ausfall \xe2\x80\x94 WP antwortet nicht";
    }
  } else {
    heartbeat_alarm_ = false;
  }
}

/* =========================================================================
 * dump_config()
 * ====================================================================== */

void EebusWpComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "EEBus WP Component::");
  ESP_LOGCONFIG(TAG, "  SHIP Port:      %d",   ship_port_);
  ESP_LOGCONFIG(TAG, "  Remote SKI:      %s",   remote_ski_.empty() ? "(auto-discover)" : remote_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Connected:      %s",   connected_ ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Heartbeat:      every %u s", kHeartbeatTimeoutSeconds);
  ESP_LOGCONFIG(TAG, "  Failsafe:       %.0f W / %u s", failsafe_limit_w_, failsafe_duration_s_);
}

/* =========================================================================
 * Public API
 * ====================================================================== */

void EebusWpComponent::set_limit(float watts) {
  if (!connected_ || !have_remote_entity_ || !eg_lpc_) {
    ESP_LOGW(TAG, "set_limit(%.0f W) — WP not connected", watts);
    return;
  }

  /* §14a: never limit below 4200 W (WP also enforces this internally) */
  if (watts > 0.0f && watts < 4200.0f) {
    ESP_LOGW(TAG, "Clamping %.0f W → 4200 W (§14a minimum)", watts);
    watts = 4200.0f;
  }

  LoadLimit limit;
  memset(&limit, 0, sizeof(limit));

  if (watts <= 0.0f) {
    limit.value.value = 99999;
    limit.value.scale = 0;
    limit.is_active   = false;
    ESP_LOGI(TAG, "Clearing WP power limit");
  } else {
    limit.value.value = (int64_t)watts;
    limit.value.scale = 0;
    limit.is_active   = true;
    ESP_LOGI(TAG, "Setting WP power limit: %.0f W", watts);
  }

  EebusError err = EgLpcSetActiveConsumptionPowerLimit(eg_lpc_, &remote_entity_addr_, &limit);
  if (err != kEebusErrorOk) {
    ESP_LOGE(TAG, "SetActiveConsumptionPowerLimit failed: %d", (int)err);
    return;
  }

  active_limit_w_ = watts > 0.0f ? watts : 0.0f;
}

/* =========================================================================
 * EgLpListenerInterface callbacks
 * ====================================================================== */

void EebusWpComponent::on_entity_connect(const EntityAddressType* addr) {
  ESP_LOGI(TAG, "WP entity connected");
  connected_          = true;
  heartbeat_alarm_    = false;
  have_remote_entity_ = true;
  if (addr) remote_entity_addr_ = *addr;
  pairing_state_      = "Verbunden mit WP";

  /* Configure failsafe on WP: if HEMS heartbeat stops, WP falls back
   * to failsafe_limit_w_ for failsafe_duration_s_ seconds, then normal. */
  ScaledValue fs_limit;
  fs_limit.value = (int64_t)failsafe_limit_w_;
  fs_limit.scale = 0;
  EgLpcSetFailsafeConsumptionActivePowerLimit(eg_lpc_, addr, &fs_limit);

  EebusDuration fs_duration;
  memset(&fs_duration, 0, sizeof(fs_duration));
  fs_duration.seconds = (int32_t)failsafe_duration_s_;
  EgLpcSetFailsafeDurationMinimum(eg_lpc_, addr, &fs_duration);

  /* Start sending heartbeat — WP requires this periodically.
   * openeebus drives the heartbeat automatically via its internal
   * FreeRTOS 1-second tick timer (device_local.c DeviceLocal1sTickCallback).
   * EgLpcStartHeartbeat() activates the HeartbeatManager for this entity. */
  EgLpcStartHeartbeat(eg_lpc_);
  ESP_LOGI(TAG, "Heartbeat started (interval: %u s)", kHeartbeatTimeoutSeconds);

  for (auto* t : connected_triggers_) t->trigger();
}

void EebusWpComponent::on_entity_disconnect(const EntityAddressType* /*addr*/) {
  ESP_LOGW(TAG, "WP entity disconnected");
  connected_          = false;
  have_remote_entity_ = false;
  active_limit_w_     = 0.0f;
  pairing_state_      = "Getrennt — suche WP...";

  if (eg_lpc_) EgLpcStopHeartbeat(eg_lpc_);

  for (auto* t : disconnected_triggers_) t->trigger();
}

void EebusWpComponent::on_power_limit_ack(float watts, bool active) {
  ESP_LOGD(TAG, "WP ACK limit %.0f W active=%s", watts, active ? "yes" : "no");
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

void EebusWpComponent::save_remote_ski_nvs_(const char* ski) {
  if (!ski) return;
  /* Allow empty string to clear NVS; reject "unknown" and suspiciously short strings */
  if (strlen(ski) > 0 && (strcmp(ski, "unknown") == 0 || strlen(ski) < 20)) {
    ESP_LOGW(TAG, "save_remote_ski_nvs_: ignoring invalid SKI '%s'", ski);
    return;
  }
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return;
  nvs_set_str(h, NVS_KEY_SKI, ski);
  nvs_commit(h);
  nvs_close(h);
}

std::string EebusWpComponent::load_remote_ski_nvs_() {
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return {};
  size_t len = 0;
  std::string result;
  if (nvs_get_str(h, NVS_KEY_SKI, nullptr, &len) == ESP_OK && len > 1) {
    result.resize(len);
    nvs_get_str(h, NVS_KEY_SKI, &result[0], &len);
    result.resize(len - 1);  /* strip NUL terminator included in len */
  }
  nvs_close(h);
  return result;
}

bool EebusWpComponent::load_or_generate_cert_() {
  mbedtls_pk_context       pk;
  mbedtls_x509write_cert   crt;
  mbedtls_entropy_context  entropy;
  mbedtls_ctr_drbg_context drbg;

  mbedtls_pk_init(&pk);
  mbedtls_x509write_crt_init(&crt);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&drbg);

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
    // Use non-deprecated serial API (mbedTLS 3.x / ESP-IDF 5.x)
    uint8_t serial_bytes[] = {0x01};
    mbedtls_x509write_crt_set_serial_raw(&crt, serial_bytes, sizeof(serial_bytes));
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
  return ok;
}

/* =========================================================================
 * start_eebus_service_() — correct pattern from openeebus examples/hems
 * ====================================================================== */

bool EebusWpComponent::start_eebus_service_(
    const uint8_t* cert_der, size_t cert_len,
    const uint8_t* key_der,  size_t key_len)
{
  /* Parse TLS certificate */
  TlsCertificateObject* tls_cert = TlsCertificateParseX509KeyPair(
      (const char*)cert_der, cert_len, (const char*)key_der, key_len);
  if (!tls_cert) { ESP_LOGE(TAG, "TlsCertificateParse failed"); return false; }

  const char* local_ski = TLS_CERTIFICATE_GET_SKI(tls_cert);
  if (local_ski) local_ski_ = local_ski;
  ESP_LOGI(TAG, "WP-EG local SKI: %s", local_ski ? local_ski : "(null)");

  /* Build service config */
  EebusServiceConfig* cfg = EebusServiceConfigCreate(
      "DIY",
      device_brand_.c_str(),
      device_model_.c_str(),
      "HEMS-WP-01",
      "EnergyManagementSystem",
      ship_port_);
  if (!cfg) { ESP_LOGE(TAG, "EebusServiceConfigCreate failed"); return false; }

  EebusServiceConfigSetRegisterAutoAccept(cfg, remote_ski_.empty());

  /* Set up ServiceReader vtable */
  SERVICE_READER_INTERFACE(&service_reader_) = &kServiceReaderMethods;
  service_reader_.self = this;

  /* Register remote WP SKI if configured */
  // (done after service start via EEBUS_SERVICE_REGISTER_REMOTE_SKI)

  /* Create service — correct 4-argument signature from eebus_service.h */
  service_ = EebusServiceCreate(cfg, "EnergyManagementSystem", tls_cert,
                                 SERVICE_READER_OBJECT(&service_reader_));
  EebusServiceConfigDelete(cfg);
  if (!service_) { ESP_LOGE(TAG, "EebusServiceCreate failed"); return false; }

  if (!remote_ski_.empty()) {
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, remote_ski_.c_str(), true);
    pairing_mode_ = false;
    ESP_LOGI(TAG, "Registered remote WP SKI: %s", remote_ski_.c_str());
  } else {
    pairing_mode_ = true;
    EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, true);
    ESP_LOGI(TAG, "WP pairing mode active — will accept first connecting device");
  }

  /* Get local SPINE device */
  DeviceLocalObject* device_local = EEBUS_SERVICE_GET_LOCAL_DEVICE(service_);
  if (!device_local) { ESP_LOGE(TAG, "GetLocalDevice failed"); return false; }

  /* Create local CEM entity with heartbeat timeout.
   * This is the critical step — without a proper entity the heartbeat
   * manager is never created and WP raises a connection fault. */
  uint32_t entity_id = 1;
  local_entity_ = EntityLocalCreate(
      device_local,
      kEntityTypeTypeCEM,      /* CEM = Controller Entity Manager (EG role) */
      &entity_id,
      1,
      kHeartbeatTimeoutSeconds /* ← drives the FreeRTOS heartbeat timer */
  );
  if (!local_entity_) { ESP_LOGE(TAG, "EntityLocalCreate failed"); return false; }

  /* Create EG/LPC use case against the entity (not the service) */
  EG_LP_LISTENER_INTERFACE(&eg_listener_) = &kEgListenerMethods;
  eg_listener_.self = this;

  eg_lpc_ = EgLpcUseCaseCreate(local_entity_, EG_LP_LISTENER_OBJECT(&eg_listener_));
  if (!eg_lpc_) { ESP_LOGE(TAG, "EgLpcUseCaseCreate failed"); return false; }

  /* Register entity with device so it is advertised via SPINE/mDNS */
  DEVICE_LOCAL_ADD_ENTITY(device_local, local_entity_);

  /* Start service — begins mDNS announcement and SHIP server */
  EEBUS_SERVICE_START(service_);
  // EEBUS_SERVICE_START returns void in this version of openeebus

  pairing_state_ = "Suche WP via mDNS...";
  ESP_LOGI(TAG, "EEBus WP service started");
  return true;
}

/* =========================================================================
 * Pairing management
 * ====================================================================== */

void EebusWpComponent::on_ship_data_exchange_(const char* ski) {
  ESP_LOGW(TAG, "WP data exchange active — SKI persisted: %s", ski);
  if (pairing_mode_) {
    pairing_mode_ = false;
    EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
    ESP_LOGI(TAG, "Pairing mode exited after successful connection");
  }
}

void EebusWpComponent::enter_pairing_mode() {
  ESP_LOGW(TAG, "Pairing mode activated");
  pairing_mode_ = true;
  if (service_) {
    if (!remote_ski_.empty())
      EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(service_, remote_ski_.c_str());
    EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, true);
  }
  pairing_state_ = "Pairing-Modus aktiv — warte auf Verbindung...";
}

void EebusWpComponent::forget_pairing() {
  ESP_LOGW(TAG, "Pairing forgotten");
  save_remote_ski_nvs_("");
  remote_ski_.clear();
  enter_pairing_mode();
}

}  // namespace eebus_wp
}  // namespace esphome
