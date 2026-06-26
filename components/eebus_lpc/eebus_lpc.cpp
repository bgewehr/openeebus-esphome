/*
 * Copyright 2025 bgewehr (bg-hems branch)
 * Licensed under the Apache License, Version 2.0
 */

#include "eebus_lpc.h"

#include <cmath>
#include <cstring>

#include "esphome/core/log.h"
#include "esphome/core/application.h"

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
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
}

#include "port/esp32/websocket/websocket_server_esp32.h"

namespace esphome {
namespace eebus_lpc {

static const char* NVS_NS        = "eebus";
static const char* NVS_KEY_CERT  = "cert_der";
static const char* NVS_KEY_KEY   = "key_der";
static const char* NVS_KEY_TSKI  = "trusted_ski";  /* persisted trusted SKI */

static const uint32_t kHeartbeatTimeoutMs  = 60000;
static const uint32_t kPairingWindowMs     = 120000; /* 2 min to confirm */

/* =========================================================================
 * setup()
 * ====================================================================== */

void EebusLpcComponent::setup() {
  ESP_LOGI(TAG, "Setting up EEBus LPC (§14a EnWG)");

  uint8_t* cert = nullptr; size_t cert_len = 0;
  uint8_t* key  = nullptr; size_t key_len  = 0;

  if (!load_cert_nvs_(&cert, &cert_len, &key, &key_len)) {
    ESP_LOGI(TAG, "Generating self-signed certificate...");
    if (!load_or_generate_cert_() || !load_cert_nvs_(&cert, &cert_len, &key, &key_len)) {
      ESP_LOGE(TAG, "Certificate setup failed");
      mark_failed(); return;
    }
  }

  /* If no remote_ski in YAML, try NVS (persisted from previous pairing) */
  if (remote_ski_.empty()) {
    remote_ski_ = load_trusted_ski_();
    if (!remote_ski_.empty()) {
      ESP_LOGI(TAG, "Loaded trusted SKI from NVS: %s", remote_ski_.c_str());
    }
  }

  if (!start_eebus_service_(cert, cert_len, key, key_len)) {
    ESP_LOGE(TAG, "Failed to start openeebus service");
    free(cert); free(key);
    mark_failed(); return;
  }

  free(cert); free(key);

  update_pairing_state_(remote_ski_.empty() ? "Warte auf Verbindung (Pairing-Modus)" : "Warte auf Steuerbox");
  ESP_LOGI(TAG, "EEBus LPC ready — local SKI: %s", local_ski_.c_str());
  ESP_LOGI(TAG, "Remote SKI: %s", remote_ski_.empty() ? "(auto-pairing)" : remote_ski_.c_str());
}

/* =========================================================================
 * loop() — heartbeat watchdog + pairing window timeout
 * ====================================================================== */

void EebusLpcComponent::loop() {
  if (!service_) return;

  uint32_t now = millis();

  /* Heartbeat watchdog */
  if (limit_active_ && !heartbeat_lost_ && last_heartbeat_ms_ > 0) {
    if ((now - last_heartbeat_ms_) > kHeartbeatTimeoutMs) {
      ESP_LOGW(TAG, "Heartbeat lost — applying failsafe %.0f W", failsafe_limit_w_);
      heartbeat_lost_ = true;
      on_power_limit_receive(failsafe_limit_w_, true);
    }
  }

  /* Pairing window timeout: reject if user doesn't confirm within 2 min */
  static uint32_t pairing_start_ms = 0;
  if (pairing_window_open_) {
    if (pairing_start_ms == 0) pairing_start_ms = now;
    if ((now - pairing_start_ms) > kPairingWindowMs) {
      ESP_LOGW(TAG, "Pairing window expired, rejecting %s", pending_remote_ski_.c_str());
      reject_pairing();
      pairing_start_ms = 0;
    }
  } else {
    pairing_start_ms = 0;
  }
}

/* =========================================================================
 * dump_config()
 * ====================================================================== */

void EebusLpcComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "EEBus LPC Component (§14a EnWG):");
  ESP_LOGCONFIG(TAG, "  SHIP Port:      %d",   ship_port_);
  ESP_LOGCONFIG(TAG, "  Local SKI:      %s",   local_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Remote SKI:     %s",   remote_ski_.empty() ? "(pairing mode)" : remote_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Paired:         %s",   is_paired() ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Failsafe:       %.0f W", failsafe_limit_w_);
  ESP_LOGCONFIG(TAG, "  Device:         %s / %s / %s",
                device_brand_.c_str(), device_type_.c_str(), device_model_.c_str());
}

/* =========================================================================
 * SHIP event callbacks — called from openeebus C layer
 * ====================================================================== */

void EebusLpcComponent::on_remote_ski_connected(const char* ski) {
  ESP_LOGI(TAG, "Remote SKI connected: %s", ski);
  pending_remote_ski_ = ski;

  if (!remote_ski_.empty() && remote_ski_ == ski) {
    /* Known SKI — auto-trust */
    ESP_LOGI(TAG, "Known SKI, auto-trusting");
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, ski, true);
    update_pairing_state_("Verbunden: " + std::string(ski));
  } else if (remote_ski_.empty()) {
    /* Pairing mode — ask user */
    ESP_LOGW(TAG, "Unknown SKI wants to pair: %s", ski);
    pairing_window_open_ = true;
    update_pairing_state_("Pairing-Anfrage: " + std::string(ski));
    /* Fire trigger so YAML automation can notify user */
    for (auto* t : pairing_request_triggers_) {
      t->trigger(std::string(ski));
    }
  } else {
    /* Wrong SKI — reject */
    ESP_LOGW(TAG, "Untrusted SKI %s, rejecting", ski);
    EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service_, ski);
    pending_remote_ski_.clear();
    update_pairing_state_("Abgelehnt: " + std::string(ski));
  }
}

void EebusLpcComponent::on_remote_ski_disconnected(const char* ski) {
  ESP_LOGI(TAG, "Remote SKI disconnected: %s", ski);
  if (paired_remote_ski_ == ski)  paired_remote_ski_.clear();
  if (pending_remote_ski_ == ski) pending_remote_ski_.clear();
  pairing_window_open_ = false;

  /* Clear limit if active */
  if (limit_active_) {
    limit_active_    = false;
    current_limit_w_ = 0.0f;
    for (auto* t : limit_cleared_triggers_) t->trigger();
  }
  update_pairing_state_("Getrennt");
}

void EebusLpcComponent::on_ship_state_update(const char* ski, SmeState state) {
  ESP_LOGD(TAG, "SHIP state update ski=%s state=%d", ski, (int)state);

  switch (state) {
    case kSmeStateApproved:
    case kDataExchange:
      if (paired_remote_ski_ != ski) {
        paired_remote_ski_  = ski;
        pending_remote_ski_.clear();
        pairing_window_open_ = false;
        update_pairing_state_("Gepairt & verbunden: " + std::string(ski));
        ESP_LOGI(TAG, "Pairing complete with %s", ski);
      }
      break;

    case kSmeHelloStateAbort:
    case kSmeHelloStateRejected:
    case kSmeStateError:
      ESP_LOGW(TAG, "SHIP handshake failed/rejected for %s (state %d)", ski, (int)state);
      if (pending_remote_ski_ == ski) pending_remote_ski_.clear();
      pairing_window_open_ = false;
      update_pairing_state_("Handshake fehlgeschlagen");
      break;

    default:
      break;
  }
}

void EebusLpcComponent::on_ship_id_update(const char* ski, const char* ship_id) {
  ESP_LOGI(TAG, "SHIP ID update: ski=%s ship_id=%s", ski, ship_id ? ship_id : "(null)");
}

bool EebusLpcComponent::is_waiting_for_trust_allowed(const char* /*ski*/) {
  /* Allow trust window when in pairing mode (no remote_ski configured) */
  return remote_ski_.empty() || pairing_window_open_;
}

/* =========================================================================
 * Public pairing control — called from YAML buttons
 * ====================================================================== */

void EebusLpcComponent::accept_pairing() {
  if (pending_remote_ski_.empty()) {
    ESP_LOGW(TAG, "accept_pairing() called but no pending SKI");
    return;
  }
  const std::string ski = pending_remote_ski_;
  ESP_LOGI(TAG, "User accepted pairing with %s", ski.c_str());

  EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, ski.c_str(), true);
  pairing_window_open_ = false;

  /* Persist trusted SKI to NVS so it survives reboot */
  save_trusted_ski_(ski);
  remote_ski_ = ski;

  update_pairing_state_("Pairing akzeptiert: " + ski);
}

void EebusLpcComponent::reject_pairing() {
  if (pending_remote_ski_.empty()) return;
  const std::string ski = pending_remote_ski_;
  ESP_LOGI(TAG, "Rejecting pairing with %s", ski.c_str());

  EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service_, ski.c_str());
  pending_remote_ski_.clear();
  pairing_window_open_ = false;
  update_pairing_state_("Pairing abgelehnt");
}

void EebusLpcComponent::forget_pairing(const std::string& ski) {
  ESP_LOGI(TAG, "Forgetting trusted SKI: %s", ski.c_str());
  if (service_) {
    EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(service_, ski.c_str());
  }
  if (paired_remote_ski_ == ski)  paired_remote_ski_.clear();
  if (remote_ski_ == ski)         remote_ski_.clear();

  /* Clear NVS */
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
    nvs_erase_key(h, NVS_KEY_TSKI);
    nvs_commit(h);
    nvs_close(h);
  }
  update_pairing_state_("Pairing zurückgesetzt — Warte auf neue Verbindung");
}

/* =========================================================================
 * LPC callbacks
 * ====================================================================== */

void EebusLpcComponent::on_power_limit_receive(float limit_w, bool is_active) {
  ESP_LOGI(TAG, "LPC: %.0f W, active=%s", limit_w, is_active ? "yes" : "no");
  current_limit_w_ = limit_w;
  heartbeat_lost_  = false;
  last_heartbeat_ms_ = millis();

  if (is_active && !limit_active_) {
    limit_active_ = true;
    for (auto* t : limit_active_triggers_) t->trigger(limit_w);
  } else if (!is_active && limit_active_) {
    limit_active_    = false;
    current_limit_w_ = 0.0f;
    for (auto* t : limit_cleared_triggers_) t->trigger();
  } else if (is_active) {
    for (auto* t : limit_active_triggers_) t->trigger(limit_w);
  }
}

void EebusLpcComponent::on_failsafe_limit_receive(float limit_w) {
  ESP_LOGW(TAG, "Failsafe limit: %.0f W", limit_w);
  failsafe_limit_w_ = limit_w;
}

void EebusLpcComponent::on_heartbeat_receive(uint64_t counter) {
  last_heartbeat_ms_ = millis();
  heartbeat_lost_    = false;
  ESP_LOGV(TAG, "Heartbeat %" PRIu64, counter);
}

/* =========================================================================
 * NVS helpers
 * ====================================================================== */

void EebusLpcComponent::save_trusted_ski_(const std::string& ski) {
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return;
  nvs_set_str(h, NVS_KEY_TSKI, ski.c_str());
  nvs_commit(h);
  nvs_close(h);
  ESP_LOGI(TAG, "Trusted SKI saved to NVS: %s", ski.c_str());
}

std::string EebusLpcComponent::load_trusted_ski_() {
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return {};
  size_t len = 0;
  if (nvs_get_str(h, NVS_KEY_TSKI, nullptr, &len) != ESP_OK || len == 0) {
    nvs_close(h); return {};
  }
  std::string ski(len, '\0');
  nvs_get_str(h, NVS_KEY_TSKI, &ski[0], &len);
  nvs_close(h);
  ski.resize(len > 0 ? len - 1 : 0); /* strip null terminator */
  return ski;
}

bool EebusLpcComponent::store_cert_nvs_(
    const uint8_t* cert, size_t cert_len,
    const uint8_t* key,  size_t key_len)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return false;
  bool ok = (nvs_set_blob(h, NVS_KEY_CERT, cert, cert_len) == ESP_OK) &&
            (nvs_set_blob(h, NVS_KEY_KEY,  key,  key_len)  == ESP_OK) &&
            (nvs_commit(h) == ESP_OK);
  nvs_close(h);
  return ok;
}

bool EebusLpcComponent::load_cert_nvs_(
    uint8_t** cert, size_t* cert_len,
    uint8_t** key,  size_t* key_len)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return false;
  size_t cl = 0, kl = 0;
  bool ok = false;
  do {
    if (nvs_get_blob(h, NVS_KEY_CERT, nullptr, &cl) != ESP_OK || cl == 0) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  nullptr, &kl) != ESP_OK || kl == 0) break;
    *cert = (uint8_t*)malloc(cl); *key = (uint8_t*)malloc(kl);
    if (!*cert || !*key) { free(*cert); free(*key); break; }
    if (nvs_get_blob(h, NVS_KEY_CERT, *cert, &cl) != ESP_OK) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  *key,  &kl) != ESP_OK) break;
    *cert_len = cl; *key_len = kl; ok = true;
  } while (false);
  nvs_close(h);
  if (!ok) { free(*cert); free(*key); *cert = *key = nullptr; }
  return ok;
}

/* =========================================================================
 * Certificate generation
 * ====================================================================== */

bool EebusLpcComponent::load_or_generate_cert_() {
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
    const char* pers = "eebus_hems_14a";
    if (mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char*)pers, strlen(pers)) != 0) break;
    if (mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0 ||
        mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(pk),
                            mbedtls_ctr_drbg_random, &drbg) != 0) break;

    uint8_t key_buf[2048]; memset(key_buf, 0, sizeof(key_buf));
    int key_len = mbedtls_pk_write_key_der(&pk, key_buf, sizeof(key_buf));
    if (key_len <= 0) break;
    uint8_t* key_p = key_buf + sizeof(key_buf) - key_len;

    std::string subj = "CN=" + device_model_ + ",O=" + device_brand_ + ",C=DE";
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
    if (ok) ESP_LOGI(TAG, "Certificate generated (cert=%d B, key=%d B)", cert_len, key_len);
  } while (false);

  mbedtls_pk_free(&pk);
  mbedtls_x509write_crt_free(&crt);
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&drbg);
  mbedtls_mpi_free(&serial);
  return ok;
}

/* =========================================================================
 * Start openeebus service
 * ====================================================================== */

/* C-compatible ShipNodeReader vtable — bridges SHIP events to C++ component */
struct HemShipNodeReader {
  ShipNodeReaderObject   obj;    /* must be first */
  EebusLpcComponent*     self;
};

extern "C" {
static void SNR_Destruct(ShipNodeReaderObject*) {}
static void SNR_OnRemoteSkiConnected(ShipNodeReaderObject* o, const char* ski) {
  reinterpret_cast<HemShipNodeReader*>(o)->self->on_remote_ski_connected(ski);
}
static void SNR_OnRemoteSkiDisconnected(ShipNodeReaderObject* o, const char* ski) {
  reinterpret_cast<HemShipNodeReader*>(o)->self->on_remote_ski_disconnected(ski);
}
static DataReaderObject* SNR_SetupRemoteDevice(
    ShipNodeReaderObject* /*o*/, const char* /*ski*/, DataWriterObject* /*dw*/)
{ return nullptr; /* handled by openeebus service internally */ }
static void SNR_OnRemoteServicesUpdate(ShipNodeReaderObject*, const Vector*) {}
static void SNR_OnShipIdUpdate(ShipNodeReaderObject* o, const char* ski, const char* ship_id) {
  reinterpret_cast<HemShipNodeReader*>(o)->self->on_ship_id_update(ski, ship_id);
}
static void SNR_OnShipStateUpdate(ShipNodeReaderObject* o, const char* ski, SmeState state) {
  reinterpret_cast<HemShipNodeReader*>(o)->self->on_ship_state_update(ski, state);
}
static bool SNR_IsWaitingForTrustAllowed(ShipNodeReaderObject* o, const char* ski) {
  return reinterpret_cast<HemShipNodeReader*>(o)->self->is_waiting_for_trust_allowed(ski);
}
static const ShipNodeReaderInterface kSnrMethods = {
  .destruct                  = SNR_Destruct,
  .on_remote_ski_connected   = SNR_OnRemoteSkiConnected,
  .on_remote_ski_disconnected= SNR_OnRemoteSkiDisconnected,
  .setup_remote_device       = SNR_SetupRemoteDevice,
  .on_remote_services_update = SNR_OnRemoteServicesUpdate,
  .on_ship_id_update         = SNR_OnShipIdUpdate,
  .on_ship_state_update      = SNR_OnShipStateUpdate,
  .is_waiting_for_trust_allowed = SNR_IsWaitingForTrustAllowed,
};
} // extern "C"

bool EebusLpcComponent::start_eebus_service_(
    const uint8_t* cert_der, size_t cert_len,
    const uint8_t* key_der,  size_t key_len)
{
  TlsCertificateObject* tls_cert = TlsCertificateParseX509KeyPair(
      (const char*)cert_der, cert_len, (const char*)key_der, key_len);
  if (!tls_cert) { ESP_LOGE(TAG, "TlsCertificateParse failed"); return false; }

  const char* ski = TLS_CERTIFICATE_GET_SKI(tls_cert);
  local_ski_ = ski ? ski : "";
  ESP_LOGI(TAG, "Local SKI: %s", local_ski_.c_str());

  EebusDeviceInfo device_info = {
    .brand = device_brand_.c_str(),
    .type  = device_type_.c_str(),
    .model = device_model_.c_str(),
  };

  WebsocketCreatorObject* ws_creator = WebsocketServerCreatorEsp32Create(
      ship_port_, cert_der, cert_len, key_der, key_len);
  if (!ws_creator) { ESP_LOGE(TAG, "WS creator failed"); return false; }

  EebusServiceConfig cfg;
  EebusServiceConfigInit(&cfg);
  cfg.tls_certificate   = tls_cert;
  cfg.device_info       = &device_info;
  cfg.websocket_creator = ws_creator;
  cfg.port              = ship_port_;
  cfg.remote_ski        = remote_ski_.empty() ? nullptr : remote_ski_.c_str();

  service_ = EebusServiceCreate(&cfg);
  if (!service_) { ESP_LOGE(TAG, "EebusServiceCreate failed"); return false; }

  /* Attach our ShipNodeReader to intercept SHIP events */
  static HemShipNodeReader snr;
  SHIP_NODE_READER_INTERFACE(&snr) = &kSnrMethods;
  snr.self = this;
  /* Note: openeebus service registers its internal ShipNodeReader;
   * we hook in via the service's own reader which already delegates.
   * For a full override we'd need to patch EebusServiceConfig — see TODO. */

  /* LPC listener */
  CS_LP_LISTENER_INTERFACE(&lpc_listener_) = &kLpcListenerMethods;
  lpc_listener_.self = this;

  cs_lpc_ = CsLpcCreate(service_, CS_LP_LISTENER_OBJECT(&lpc_listener_));
  if (!cs_lpc_) { ESP_LOGE(TAG, "CsLpcCreate failed"); return false; }

  EebusError err = EEBUS_SERVICE_START(service_);
  if (err != kEebusErrorOk) { ESP_LOGE(TAG, "ServiceStart failed: %d", err); return false; }

  last_heartbeat_ms_ = millis();
  return true;
}

/* =========================================================================
 * Helper
 * ====================================================================== */

void EebusLpcComponent::update_pairing_state_(const std::string& state) {
  pairing_state_str_ = state;
  ESP_LOGI(TAG, "Pairing state: %s", state.c_str());
}

}  // namespace eebus_lpc
}  // namespace esphome
