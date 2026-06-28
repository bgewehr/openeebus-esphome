/*
 * Copyright 2025 bgewehr
 * Licensed under the Apache License, Version 2.0
 */

#include "eebus_lpc.h"

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
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
}

#include "port/esp32/websocket/websocket_server_esp32.h"

namespace esphome {
namespace eebus_lpc {

static const char* NVS_NS       = "eebus";
static const char* NVS_KEY_CERT = "cert_der";
static const char* NVS_KEY_KEY  = "key_der";
static const char* NVS_KEY_TSKI = "trusted_ski";

/* CS role: heartbeat timeout for the local EMS entity.
 * The CLS-Steuerbox sends heartbeats to us; kHeartbeatTimeoutSeconds
 * configures how long our HeartbeatManager waits before considering
 * the connection stale. 60 s matches the openeebus reference examples. */
static const uint32_t kHeartbeatTimeoutSeconds = 60;

/* Pairing window: how long the explicit pairing mode stays open */
static const uint32_t kPairingWindowMs = 300000;  /* 5 minutes */

/* Electrical connection ID for the CS LPC use case.
 * Use 0 to match the openeebus reference (hpsrv.c kHpsrvElectricalConnectionId). */
static const uint32_t kElectricalConnectionId = 0;

/* =========================================================================
 * ServiceReader C vtable
 * ====================================================================== */

struct LpcServiceReader {
  ServiceReaderObject  obj;    /* must be first */
  EebusLpcComponent*   self;
};

extern "C" {

static void LpcSR_Destruct(ServiceReaderObject*) {}

static void LpcSR_OnRemoteSkiConnected(
    ServiceReaderObject* o, EebusServiceObject* /*svc*/, const char* ski)
{
  reinterpret_cast<LpcServiceReader*>(o)->self->on_remote_ski_connected(ski);
}

static void LpcSR_OnRemoteSkiDisconnected(
    ServiceReaderObject* o, EebusServiceObject* /*svc*/, const char* ski)
{
  reinterpret_cast<LpcServiceReader*>(o)->self->on_remote_ski_disconnected(ski);
}

static void LpcSR_OnRemoteServicesUpdate(
    ServiceReaderObject*, EebusServiceObject*, const Vector*) {}

static void LpcSR_OnShipIdUpdate(
    ServiceReaderObject* o, const char* ski, const char* ship_id)
{
  reinterpret_cast<LpcServiceReader*>(o)->self->on_ship_id_update(ski, ship_id);
}

static void LpcSR_OnShipStateUpdate(
    ServiceReaderObject* o, const char* ski, SmeState state)
{
  reinterpret_cast<LpcServiceReader*>(o)->self->on_ship_state_update(ski, state);
}

static bool LpcSR_IsWaitingForTrustAllowed(
    const ServiceReaderObject* o, const char* ski)
{
  return reinterpret_cast<const LpcServiceReader*>(o)->self->is_waiting_for_trust_allowed(ski);
}

static const ServiceReaderInterface kServiceReaderMethods = {
  .destruct                     = LpcSR_Destruct,
  .on_remote_ski_connected      = LpcSR_OnRemoteSkiConnected,
  .on_remote_ski_disconnected   = LpcSR_OnRemoteSkiDisconnected,
  .on_remote_services_update    = LpcSR_OnRemoteServicesUpdate,
  .on_ship_id_update            = LpcSR_OnShipIdUpdate,
  .on_ship_state_update         = LpcSR_OnShipStateUpdate,
  .is_waiting_for_trust_allowed = LpcSR_IsWaitingForTrustAllowed,
};

}  // extern "C"

/* =========================================================================
 * setup()
 * ====================================================================== */

void EebusLpcComponent::setup() {
  ESP_LOGI(TAG, "Setting up EEBus LPC (§14a EnWG CS role)");

  uint8_t* cert = nullptr; size_t cert_len = 0;
  uint8_t* key  = nullptr; size_t key_len  = 0;

  if (!load_cert_nvs_(&cert, &cert_len, &key, &key_len)) {
    ESP_LOGI(TAG, "Generating self-signed certificate...");
    if (!load_or_generate_cert_() ||
        !load_cert_nvs_(&cert, &cert_len, &key, &key_len)) {
      ESP_LOGE(TAG, "Certificate setup failed");
      mark_failed(); return;
    }
  }

  /* If no remote_ski in YAML, try NVS (from previous pairing) */
  if (remote_ski_.empty()) {
    remote_ski_ = load_trusted_ski_();
    if (!remote_ski_.empty())
      ESP_LOGI(TAG, "Loaded trusted SKI from NVS: %s", remote_ski_.c_str());
  }

  if (!start_eebus_service_(cert, cert_len, key, key_len)) {
    ESP_LOGE(TAG, "Failed to start openeebus service");
    free(cert); free(key);
    mark_failed(); return;
  }

  free(cert); free(key);
  update_pairing_state_(remote_ski_.empty()
      ? "Kein Pairing — Pairing-Modus manuell aktivieren"
      : "Warte auf Steuerbox");
  ESP_LOGI(TAG, "EEBus LPC CS ready — local SKI: %s", local_ski_.c_str());
}

/* =========================================================================
 * loop()
 * ====================================================================== */

void EebusLpcComponent::loop() {
  if (!service_) return;

  uint32_t now = millis();

  /* Heartbeat watchdog: if paired and no heartbeat within timeout, apply failsafe */
  if (limit_active_ && !heartbeat_lost_ && last_heartbeat_ms_ > 0) {
    if ((now - last_heartbeat_ms_) > kHeartbeatTimeoutSeconds * 1000u) {
      ESP_LOGW(TAG, "Heartbeat lost — applying failsafe %.0f W", failsafe_limit_w_);
      heartbeat_lost_ = true;
      on_power_limit_receive(failsafe_limit_w_, true);
    }
  }

  /* Pairing window timeout */
  if (pairing_mode_active_ && now >= pairing_deadline_ms_) {
    ESP_LOGW(TAG, "Pairing window expired");
    pairing_mode_active_ = false;
    pairing_deadline_ms_ = 0;
    if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
    if (!pending_remote_ski_.empty()) reject_pairing();
    update_pairing_state_("Pairing-Fenster abgelaufen");
  }
}

/* =========================================================================
 * dump_config()
 * ====================================================================== */

void EebusLpcComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "EEBus LPC Component (CS role, §14a EnWG):");
  ESP_LOGCONFIG(TAG, "  SHIP Port:     %d",   ship_port_);
  ESP_LOGCONFIG(TAG, "  Local SKI:     %s",   local_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Remote SKI:    %s",   remote_ski_.empty() ? "(pairing mode)" : remote_ski_.c_str());
  ESP_LOGCONFIG(TAG, "  Paired:        %s",   is_paired() ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Failsafe:      %.0f W", failsafe_limit_w_);
  ESP_LOGCONFIG(TAG, "  Device:        %s / %s / %s",
                device_brand_.c_str(), device_type_.c_str(), device_model_.c_str());
}

/* =========================================================================
 * SHIP ServiceReader callbacks
 * ====================================================================== */

void EebusLpcComponent::on_remote_ski_connected(const char* ski) {
  ESP_LOGI(TAG, "Remote SKI connected: %s", ski);
  pending_remote_ski_ = ski;

  if (!remote_ski_.empty() && remote_ski_ == ski) {
    /* Known stored SKI — register for persistent auto-connect */
    ESP_LOGI(TAG, "Known SKI — auto-trusting");
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, ski, true);
    update_pairing_state_("Verbunden: " + std::string(ski));
  } else if (paired_remote_ski_ == ski) {
    /* SHIP DataExchange already completed via is_waiting_for_trust_allowed.
     * on_ship_state_update(kSmeStateApproved) set paired_remote_ski_ and
     * cleared pairing_mode_active_ before this callback fires — so we can no
     * longer rely on pairing_mode_active_ to detect a just-paired session.
     * Persist the SKI and register it for future auto-connect. */
    ESP_LOGI(TAG, "SHIP-approved SKI — persisting: %s", ski);
    remote_ski_ = ski;
    save_trusted_ski_(remote_ski_);
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, ski, true);
    update_pairing_state_("Verbunden: " + std::string(ski));
  } else if (pairing_mode_active_ && millis() < pairing_deadline_ms_) {
    /* Unknown SKI, explicit pairing window still open — let the user approve */
    ESP_LOGW(TAG, "Unknown SKI wants to pair: %s", ski);
    update_pairing_state_("Pairing-Anfrage: " + std::string(ski));
    for (auto* t : pairing_request_triggers_) t->trigger(std::string(ski));
  } else {
    ESP_LOGW(TAG, "Untrusted SKI %s — pairing not active, rejecting", ski);
    EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service_, ski);
    pending_remote_ski_.clear();
    update_pairing_state_("Abgelehnt: " + std::string(ski));
  }
}

void EebusLpcComponent::on_remote_ski_disconnected(const char* ski) {
  ESP_LOGI(TAG, "Remote SKI disconnected: %s", ski);
  if (paired_remote_ski_ == ski)  paired_remote_ski_.clear();
  if (pending_remote_ski_ == ski) pending_remote_ski_.clear();
  /* pairing_mode_active_ stays — allow reconnect within the window */

  if (limit_active_) {
    limit_active_ = false; current_limit_w_ = 0.0f;
    for (auto* t : limit_cleared_triggers_) t->trigger();
  }
  update_pairing_state_("Getrennt");
}

void EebusLpcComponent::on_ship_state_update(const char* ski, SmeState state) {
  ESP_LOGD(TAG, "SHIP state ski=%s state=%d", ski, (int)state);
  if (state == kSmeStateApproved || state == kDataExchange) {
    if (!ski || strcmp(ski, "unknown") == 0 || strlen(ski) < 40) {
      ESP_LOGE(TAG, "DataExchange mit ungültiger SKI '%s' — Pairing ignoriert. "
               "Prüfe CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE=y in sdkconfig.",
               ski ? ski : "null");
      return;
    }
    if (paired_remote_ski_ != ski) {
      paired_remote_ski_ = ski;
      pending_remote_ski_.clear();
      pairing_mode_active_ = false;
      pairing_deadline_ms_ = 0;
      if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
      update_pairing_state_("Gepairt & verbunden: " + std::string(ski));
    }
  } else if (state == kSmeHelloStateAbort || state == kSmeHelloStateRejected ||
             state == kSmeStateError) {
    if (pending_remote_ski_ == ski) pending_remote_ski_.clear();
    /* pairing_mode_active_ stays — allow retry within the window */
    update_pairing_state_("Handshake fehlgeschlagen");
  }
}

void EebusLpcComponent::on_ship_id_update(const char* ski, const char* ship_id) {
  ESP_LOGI(TAG, "SHIP ID: ski=%s id=%s", ski, ship_id ? ship_id : "");
}

bool EebusLpcComponent::is_waiting_for_trust_allowed(const char* /*ski*/) {
  return pairing_mode_active_ && millis() < pairing_deadline_ms_;
}

/* =========================================================================
 * Pairing control
 * ====================================================================== */

void EebusLpcComponent::enter_pairing_mode() {
  ESP_LOGW(TAG, "Pairing mode activated (window: %u s)", kPairingWindowMs / 1000);
  pairing_mode_active_ = true;
  pairing_deadline_ms_ = millis() + kPairingWindowMs;
  if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, true);
  update_pairing_state_("Pairing-Modus aktiv — warte auf CS-Verbindung...");
}

void EebusLpcComponent::accept_pairing() {
  if (pending_remote_ski_.empty()) {
    ESP_LOGW(TAG, "accept_pairing() — no pending SKI"); return;
  }
  const std::string ski = pending_remote_ski_;
  EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, ski.c_str(), true);
  pairing_mode_active_ = false;
  pairing_deadline_ms_ = 0;
  if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
  save_trusted_ski_(ski);
  remote_ski_ = ski;
  update_pairing_state_("Pairing akzeptiert: " + ski);
  ESP_LOGI(TAG, "Pairing accepted: %s", ski.c_str());
}

void EebusLpcComponent::reject_pairing() {
  if (!pending_remote_ski_.empty()) {
    EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service_, pending_remote_ski_.c_str());
    pending_remote_ski_.clear();
  }
  pairing_mode_active_ = false;
  pairing_deadline_ms_ = 0;
  if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
  update_pairing_state_("Pairing abgelehnt");
}

void EebusLpcComponent::forget_pairing(const std::string& ski) {
  if (service_) EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(service_, ski.c_str());
  if (paired_remote_ski_ == ski)  paired_remote_ski_.clear();
  if (pending_remote_ski_ == ski) pending_remote_ski_.clear();
  if (remote_ski_ == ski)         remote_ski_.clear();
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
    nvs_erase_key(h, NVS_KEY_TSKI); nvs_commit(h); nvs_close(h);
  }
  enter_pairing_mode();
}

/* =========================================================================
 * LPC listener callbacks
 * ====================================================================== */

void EebusLpcComponent::on_power_limit_receive(float limit_w, bool is_active) {
  ESP_LOGI(TAG, "LPC: %.0f W active=%s", limit_w, is_active ? "yes" : "no");
  current_limit_w_   = limit_w;
  heartbeat_lost_    = false;
  last_heartbeat_ms_ = millis();

  if (is_active && !limit_active_) {
    limit_active_ = true;
    for (auto* t : limit_active_triggers_) t->trigger(limit_w);
  } else if (!is_active && limit_active_) {
    limit_active_ = false; current_limit_w_ = 0.0f;
    for (auto* t : limit_cleared_triggers_) t->trigger();
  } else if (is_active) {
    for (auto* t : limit_active_triggers_) t->trigger(limit_w);
  }
}

void EebusLpcComponent::on_failsafe_limit_receive(float limit_w) {
  ESP_LOGW(TAG, "Failsafe limit from EG: %.0f W", limit_w);
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
  nvs_commit(h); nvs_close(h);
  ESP_LOGI(TAG, "Trusted SKI saved: %s", ski.c_str());
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
  if (!ski.empty() && ski.back() == '\0') ski.pop_back();
  return ski;
}

bool EebusLpcComponent::store_cert_nvs_(
    const uint8_t* cert, size_t cl, const uint8_t* key, size_t kl)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return false;
  bool ok = (nvs_set_blob(h, NVS_KEY_CERT, cert, cl) == ESP_OK) &&
            (nvs_set_blob(h, NVS_KEY_KEY,  key,  kl) == ESP_OK) &&
            (nvs_commit(h) == ESP_OK);
  nvs_close(h); return ok;
}

bool EebusLpcComponent::load_cert_nvs_(
    uint8_t** cert, size_t* cl, uint8_t** key, size_t* kl)
{
  nvs_handle_t h;
  if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return false;
  size_t clen = 0, klen = 0; bool ok = false;
  do {
    if (nvs_get_blob(h, NVS_KEY_CERT, nullptr, &clen) != ESP_OK || clen == 0) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  nullptr, &klen) != ESP_OK || klen == 0) break;
    *cert = (uint8_t*)malloc(clen); *key = (uint8_t*)malloc(klen);
    if (!*cert || !*key) { free(*cert); free(*key); break; }
    if (nvs_get_blob(h, NVS_KEY_CERT, *cert, &clen) != ESP_OK) break;
    if (nvs_get_blob(h, NVS_KEY_KEY,  *key,  &klen) != ESP_OK) break;
    *cl = clen; *kl = klen; ok = true;
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

  mbedtls_pk_init(&pk);
  mbedtls_x509write_crt_init(&crt);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&drbg);

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
    if (ok) ESP_LOGI(TAG, "Certificate generated (%d B cert, %d B key)", cert_len, key_len);
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

bool EebusLpcComponent::start_eebus_service_(
    const uint8_t* cert_der, size_t cert_len,
    const uint8_t* key_der,  size_t key_len)
{
  /* Parse TLS certificate — service takes ownership */
  TlsCertificateObject* tls_cert = TlsCertificateParseX509KeyPair(
      (const char*)cert_der, cert_len, (const char*)key_der, key_len);
  if (!tls_cert) { ESP_LOGE(TAG, "TlsCertificateParse failed"); return false; }

  const char* ski = TLS_CERTIFICATE_GET_SKI(tls_cert);
  local_ski_ = ski ? ski : "";
  ESP_LOGI(TAG, "Local SKI: %s", local_ski_.c_str());

  /* Build service config via Create() — no stack init */
  EebusServiceConfig* cfg = EebusServiceConfigCreate(
      "DIY",                        /* vendor_code */
      device_brand_.c_str(),        /* device_brand */
      device_model_.c_str(),        /* device_model */
      "HEMS-CS-01",                 /* serial_number */
      "EnergyManagementSystem",     /* device_type */
      ship_port_);
  if (!cfg) { ESP_LOGE(TAG, "EebusServiceConfigCreate failed"); return false; }

  /* Set alternate identifier for mDNS discovery (matches openeebus HEMS example) */
  std::string alt_id = device_brand_ + "-" + device_model_ + "-HEMS-CS-01";
  EebusServiceConfigSetAlternateIdentifier(cfg, alt_id.c_str());

  EebusServiceConfigSetRegisterAutoAccept(cfg, false);  /* pairing requires explicit activation */

  /* ServiceReader vtable */
  SERVICE_READER_INTERFACE(&service_reader_.obj) = &kServiceReaderMethods;
  service_reader_.self = this;

  /* Create service — 4-argument signature */
  service_ = EebusServiceCreate(cfg, "EnergyManagementSystem", tls_cert,
                                 SERVICE_READER_OBJECT(&service_reader_.obj));
  EebusServiceConfigDelete(cfg);
  if (!service_) { ESP_LOGE(TAG, "EebusServiceCreate failed"); return false; }

  if (!remote_ski_.empty() && remote_ski_ != "unknown" && remote_ski_.length() >= 40) {
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, remote_ski_.c_str(), true);
    ESP_LOGI(TAG, "Registered remote LPC SKI: %s", remote_ski_.c_str());
  } else if (!remote_ski_.empty()) {
    ESP_LOGW(TAG, "Ignoring invalid remote SKI from config: '%s'", remote_ski_.c_str());
    remote_ski_.clear();
  }

  /* Create local entity — CS acts as EnergyManagementSystem entity.
   * kHeartbeatTimeoutSeconds activates the HeartbeatManager so openeebus
   * can respond to heartbeats from the CLS-Steuerbox (Scenario 3). */
  DeviceLocalObject* device_local = EEBUS_SERVICE_GET_LOCAL_DEVICE(service_);
  if (!device_local) { ESP_LOGE(TAG, "GetLocalDevice failed"); return false; }

  uint32_t entity_id = 1;
  local_entity_ = EntityLocalCreate(
      device_local,
      kEntityTypeTypeCEM,
      &entity_id, 1,
      kHeartbeatTimeoutSeconds);
  if (!local_entity_) { ESP_LOGE(TAG, "EntityLocalCreate failed"); return false; }

  /* Create CS LPC use case — needs entity + electrical connection ID */
  CS_LP_LISTENER_INTERFACE(&lpc_listener_) = &kLpcListenerMethods;
  lpc_listener_.self = this;

  cs_lpc_ = CsLpcUseCaseCreate(
      local_entity_,
      (ElectricalConnectionIdType)kElectricalConnectionId,
      CS_LP_LISTENER_OBJECT(&lpc_listener_));
  if (!cs_lpc_) { ESP_LOGE(TAG, "CsLpcUseCaseCreate failed"); return false; }

  EEBUS_SERVICE_START(service_);
  // EEBUS_SERVICE_START returns void in this version of openeebus

  last_heartbeat_ms_ = millis();
  return true;
}

void EebusLpcComponent::update_pairing_state_(const std::string& state) {
  pairing_state_str_ = state;
  ESP_LOGI(TAG, "Pairing state: %s", state.c_str());
}

}  // namespace eebus_lpc
}  // namespace esphome
