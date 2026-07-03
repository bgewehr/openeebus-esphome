/*
 * Copyright 2025 bgewehr (bg-hems branch)
 * Licensed under the Apache License, Version 2.0
 */

#include "eebus_wp.h"

#include <cmath>
#include <cstdarg>
#include <cstring>
#include <ctime>

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
#include "src/spine/model/node_management_types.h"
#include "src/spine/events/events.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "src/use_case/actor/ma/mpc/ma_mpc.h"
#include "src/use_case/model/load_limit_types.h"
}

#include "port/esp32/websocket/websocket_server_esp32.h"

/* ── SPINE event handler for initial exchange logging (item 2 of todo) ───── */

static const char* spine_actor_name(int a) {
  switch (a) {
    case 2:  return "CEM";
    case 4:  return "Compressor";
    case 5:  return "ControllableSystem";
    case 9:  return "EnergyGuard";
    case 13: return "HeatPump";
    case 18: return "MonitoredUnit";
    case 19: return "MonitoringAppliance";
    default: return "?";
  }
}

static const char* spine_uc_name(int n) {
  switch (n) {
    case 14: return "limitationOfPowerConsumption";
    case 25: return "monitoringOfPowerConsumption";
    case 30: return "optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility";
    default: return "?";
  }
}

static void spine_event_handler(const EventPayload* payload, void*) {
  if (!payload) return;
  const char* ski = payload->ski ? payload->ski : "?";
  switch (payload->event_type) {
    case kEventTypeUseCaseChange: {
      const UseCaseFilterType* f = payload->use_case_filter;
      if (!f) break;
      const char* change = (payload->change_type == kElementChangeAdd)    ? "add"
                         : (payload->change_type == kElementChangeUpdate)  ? "update"
                         :                                                   "remove";
      ESP_LOGD("eebus", "SPINE use-case %s from %s: actor=%d(%s) useCase=%d(%s)",
               change, ski,
               (int)f->actor, spine_actor_name(f->actor),
               (int)f->use_case_name_id, spine_uc_name(f->use_case_name_id));
      break;
    }
    case kEventTypeEntityChange:
      if (payload->change_type == kElementChangeAdd)
        ESP_LOGD("eebus", "SPINE entity added from %s", ski);
      break;
    case kEventTypeDeviceChange:
      if (payload->change_type == kElementChangeAdd)
        ESP_LOGD("eebus", "SPINE device discovered: ski=%s", ski);
      break;
    default: break;
  }
}

/* Bridge: lets openeebus C files emit logs through ESPHome's log pipeline
 * (visible in the web frontend) instead of the raw ESP-IDF serial logger. */
extern "C" void eebus_log_d(const char* tag, int line, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  esphome::esp_log_vprintf_(ESPHOME_LOG_LEVEL_DEBUG, tag, line, fmt, args);
  va_end(args);
}

namespace esphome {
namespace eebus_wp {

static const char* NVS_NS        = "eebus_wp";
static const char* NVS_KEY_CERT  = "cert_der";
static const char* NVS_KEY_KEY   = "key_der";
static const char* NVS_KEY_SKI   = "remote_ski";

/* Outbound heartbeat timeout declared to K40RF in the DeviceDiagnosis heartbeat
 * data (SPINE spec standard for HEMS). heartbeat_manager sends every
 * timeout*3/4 = 45 s, well within the 60 s window. */
static const uint32_t kHeartbeatTimeoutSeconds = 60;

/* Inbound alarm: how long without a heartbeat from K40RF before we warn.
 * K40RF sends its own heartbeat every ~40 s; 3× that gives a safe margin. */
static const uint32_t kInboundHeartbeatAlarmMs = 120000u;

/* Pairing window: how long the explicit pairing mode stays open */
static const uint32_t kPairingWindowMs = 300000;  /* 5 minutes */

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
  (void)svc;
  /* In EEBus, both the HEMS (EG role) and K40rf (CS role) connect to each other
   * via "auto" SHIP role.  Connection initiation happens via the startup
   * EEBUS_SERVICE_REGISTER_REMOTE_SKI call (known SKI) or inbound from K40rf
   * via IsWaitingForTrustAllowed (pairing).  Do NOT call REGISTER_REMOTE_SKI
   * here — the reference openeebus/examples/hems/hems.c does nothing in this
   * callback and triggering an outbound attempt from every mDNS update causes
   * spurious connections in the wrong direction. */
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  size_t n = VectorGetSize(entries);
  ESP_LOGD("eebus", "mDNS WP scan: %zu entr%s visible (periodic browser, ~15 s interval)",
           n, n == 1 ? "y" : "ies");
  for (size_t i = 0; i < n; i++) {
    const MdnsEntry* entry = (const MdnsEntry*)VectorGetElement(entries, i);
    const char* ski  = MdnsEntryGetSki(entry);
    const char* host = MdnsEntryGetHost(entry) ? MdnsEntryGetHost(entry) : "?";
    if (ski && ski[0] != '\0') {
      ESP_LOGD("eebus", "  mDNS[%zu]: ski=%s host=%s", i, ski, host);
    }
  }
  if (!r->self->pairing_mode_active_) return;
  if (!r->self->remote_ski_.empty()) return;  /* already paired */

  for (size_t i = 0; i < n; i++) {
    const MdnsEntry* entry = (const MdnsEntry*)VectorGetElement(entries, i);
    const char* ski = MdnsEntryGetSki(entry);
    if (!ski || ski[0] == '\0') continue;
    if (r->self->local_ski_ == ski) continue;
    const char* host = MdnsEntryGetHost(entry) ? MdnsEntryGetHost(entry) : "?";
    ESP_LOGI("eebus_wp", "mDNS: WP sichtbar ski=%s host=%s — warte auf eingehende Verbindung",
             ski, host);
    r->self->pairing_state_ = "WP sichtbar: " + std::string(ski) + " — warte auf Verbindung";
    break;
  }
}

static void SR_OnShipIdUpdate(
    ServiceReaderObject* o, const char* ski, const char* ship_id)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  ESP_LOGD("eebus_wp", "SHIP ID update ski=%s id=%s", ski, ship_id ? ship_id : "");
  if (ship_id && ship_id[0] != '\0' && ski && r->self->remote_ski_ == ski) {
    r->self->device_label_ = ship_id;
    ESP_LOGI("eebus_wp", "WP device name: %s", ship_id);
  }
}

static void SR_OnShipStateUpdate(
    ServiceReaderObject* o, const char* ski, SmeState state)
{
  auto* r = reinterpret_cast<WpServiceReader*>(o);
  ESP_LOGW("eebus_wp", "SHIP state ski=%s state=%d", ski, (int)state);
  if (state == kDataExchange) {
    /* Reject connections where TLS peer cert extraction failed.
     * Root cause: CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE not set in sdkconfig. */
    if (!ski || strcmp(ski, "unknown") == 0 || strlen(ski) < 40) {
      ESP_LOGE("eebus_wp", "DataExchange mit ungültiger SKI '%s' — Pairing ignoriert. "
               "Prüfe CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE=y in sdkconfig.",
               ski ? ski : "null");
      return;
    }
    r->self->remote_ski_ = ski;
    r->self->pairing_state_ = "Gepairt: " + std::string(ski);
    ESP_LOGW("eebus_wp", "WP pairing approved, remote SKI=%s", ski);
    r->self->save_remote_ski_nvs_(ski);
    r->self->on_ship_data_exchange_(ski);
  }
}

static bool SR_IsWaitingForTrustAllowed(const ServiceReaderObject* o, const char* ski) {
  if (!ski || strcmp(ski, "unknown") == 0 || strlen(ski) < 20) return false;
  const auto* self = reinterpret_cast<const WpServiceReader*>(o)->self;
  return self->pairing_mode_active_ && millis() < self->pairing_deadline_ms_;
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

  /* Check if K40RF has sent us a heartbeat within 2× the declared timeout.
   * EgLpcIsHeartbeatWithinDuration is broken (passes NULL remote entity →
   * always returns false), so we track the last received heartbeat ourselves
   * via on_heartbeat_received_() / last_heartbeat_ms_.
   * Grace period: allow up to 2× timeout after connect before alarming —
   * K40RF's heartbeat timer runs continuously and the first beat can arrive
   * up to 1× timeout after our connection is established. */
  const uint32_t hb_timeout_ms = kInboundHeartbeatAlarmMs;
  const bool grace   = connected_ && ((millis() - connected_since_ms_) < hb_timeout_ms);
  const bool hb_ok   = (last_heartbeat_ms_ != 0) &&
                       ((millis() - last_heartbeat_ms_) < hb_timeout_ms);
  if (connected_ && !grace && !hb_ok) {
    if (!heartbeat_alarm_) {
      ESP_LOGW(TAG, "WP heartbeat overdue \xe2\x80\x94 connection may be stale");
      heartbeat_alarm_ = true;
    }
  } else {
    if (heartbeat_alarm_ && connected_) {
      ESP_LOGI(TAG, "WP heartbeat recovered");
    }
    heartbeat_alarm_ = false;
  }

  /* Pairing window timeout */
  uint32_t now = millis();
  if (pairing_mode_active_ && now >= pairing_deadline_ms_) {
    ESP_LOGW(TAG, "Pairing window expired");
    pairing_mode_active_ = false;
    pairing_deadline_ms_ = 0;
    if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
    pairing_state_ = "Pairing-Fenster abgelaufen";
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
  ESP_LOGCONFIG(TAG, "  SPINE local:    device=EnergyManagementSystem entity=CEM(id=1) heartbeat=%us", kHeartbeatTimeoutSeconds);
  ESP_LOGCONFIG(TAG, "  SPINE use-case: HEMS=EnergyGuard(9) <-> WP=ControllableSystem(5) / limitationOfPowerConsumption(14) [EG/LPC]");
  ESP_LOGCONFIG(TAG, "  SPINE use-case: HEMS=MonitoringAppliance(19) <-> WP=MonitoredUnit(18) / monitoringOfPowerConsumption(25) [MA/MPC]");
  ESP_LOGCONFIG(TAG, "  SPINE negotiated: %s", active_use_cases().c_str());
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
  connected_since_ms_ = millis();
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

  // Send initial "no limit active" — K40RF only shows "EEBus verbunden" after
  // receiving at least one ActiveConsumptionPowerLimit write from the HEMS.
  clear_limit();

  for (auto* t : connected_triggers_) t->trigger();
}

void EebusWpComponent::on_entity_disconnect(const EntityAddressType* /*addr*/) {
  ESP_LOGW(TAG, "WP entity disconnected");
  connected_          = false;
  mpc_connected_      = false;
  last_heartbeat_ms_  = 0;
  have_remote_entity_ = false;
  active_limit_w_     = 0.0f;
  pairing_state_      = "Getrennt — suche WP...";

  /* Do NOT stop the heartbeat on disconnect — eebus-go never stops it.
   * Keeping it running ensures the feature data is always fresh for the
   * next time K40RF connects and subscribes. */

  for (auto* t : disconnected_triggers_) t->trigger();
}

void EebusWpComponent::on_power_limit_ack(float watts, bool active) {
  ESP_LOGD(TAG, "WP ACK limit %.0f W active=%s", watts, active ? "yes" : "no");
}

void EebusWpComponent::on_mpc_measurement(float watts) {
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
    if (!*c || !*k) { free(*c); *c = nullptr; free(*k); *k = nullptr; break; }
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

  EebusServiceConfigSetRegisterAutoAccept(cfg, false);  /* pairing requires explicit activation */

  /* Set up ServiceReader vtable */
  SERVICE_READER_INTERFACE(&service_reader_) = &kServiceReaderMethods;
  service_reader_.self = this;

  /* Register remote WP SKI if configured */
  // (done after service start via EEBUS_SERVICE_REGISTER_REMOTE_SKI)

  /* Role "auto" (kShipRoleAuto): HEMS can both initiate outbound and accept inbound.
   * Simultaneous-connect races are resolved by SKI tie-breaking in ship_node.c,
   * matching the ship-go v0.6.0 keepThisConnection approach. */
  service_ = EebusServiceCreate(cfg, "auto", tls_cert,
                                 SERVICE_READER_OBJECT(&service_reader_));
  EebusServiceConfigDelete(cfg);
  if (!service_) { ESP_LOGE(TAG, "EebusServiceCreate failed"); return false; }

  if (!remote_ski_.empty() && remote_ski_ != "unknown" && remote_ski_.length() >= 40) {
    EEBUS_SERVICE_REGISTER_REMOTE_SKI(service_, remote_ski_.c_str(), true);
    ESP_LOGI(TAG, "Registered remote WP SKI: %s", remote_ski_.c_str());
  } else {
    if (!remote_ski_.empty()) {
      ESP_LOGW(TAG, "Ignoring invalid remote SKI from config: '%s'", remote_ski_.c_str());
      remote_ski_.clear();
    }
    ESP_LOGI(TAG, "No remote SKI — pairing mode must be activated explicitly");
  }
  pairing_mode_active_ = false;
  pairing_deadline_ms_ = 0;

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

  /* MaMpc (Monitoring of Power Consumption): required so the WP can report its
   * power draw to the HEMS and shows "EEBus verbunden" on its own display.
   * The Go bridge (eebus-ha-bridge) registers this as its "Monitoring" use case. */
  MA_MPC_LISTENER_INTERFACE(&mpc_listener_) = &kMpcListenerMethods;
  mpc_listener_.self = this;
  ma_mpc_ = MaMpcUseCaseCreate(local_entity_, MA_MPC_LISTENER_OBJECT(&mpc_listener_));
  if (!ma_mpc_) { ESP_LOGW(TAG, "MaMpcUseCaseCreate failed — WP may not show EEBus verbunden"); }

  /* Register entity with device so it is advertised via SPINE/mDNS */
  DEVICE_LOCAL_ADD_ENTITY(device_local, local_entity_);

  /* Subscribe so remote SPINE announcements from K40RF appear in log under tag "eebus" */
  EventSubscribe(kEventHandlerLevelApplication, spine_event_handler, nullptr);

  /* Service start is deferred to refresh_heartbeat() (called on the first
   * on_time_sync event from SNTP or HA).  This guarantees K40RF cannot
   * subscribe while the stored DeviceDiagnosis heartbeat data still carries
   * an epoch timestamp — the root cause of its persistent "no connection"
   * display state. */
  ESP_LOGI(TAG, "EEBus WP setup complete — awaiting time sync before service start");
  return true;
}

/* =========================================================================
 * Pairing management
 * ====================================================================== */

void EebusWpComponent::on_ship_data_exchange_(const char* ski) {
  ESP_LOGW(TAG, "WP data exchange active — SKI persisted: %s", ski);
  if (pairing_mode_active_) {
    pairing_mode_active_ = false;
    pairing_deadline_ms_ = 0;
    if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, false);
    ESP_LOGI(TAG, "Pairing mode exited after successful connection");
  }
}

void EebusWpComponent::enter_pairing_mode() {
  ESP_LOGW(TAG, "Pairing mode activated (window: %u s)", kPairingWindowMs / 1000);
  pairing_mode_active_ = true;
  pairing_deadline_ms_ = millis() + kPairingWindowMs;
  if (service_) EEBUS_SERVICE_SET_PAIRING_POSSIBLE(service_, true);
  pairing_state_ = "Pairing-Modus aktiv — warte auf Verbindung...";
}

void EebusWpComponent::forget_pairing() {
  ESP_LOGW(TAG, "Pairing forgotten");
  save_remote_ski_nvs_("");
  remote_ski_.clear();
  enter_pairing_mode();
}

void EebusWpComponent::refresh_heartbeat() {
  if (!eg_lpc_) return;
  if (!time_synced_) {
    time_synced_ = true;
    EgLpcStartHeartbeat(eg_lpc_);  /* store valid timestamp before any subscriber arrives */
  }
  if (!service_started_ && service_) {
    service_started_ = true;
    /* Startup announcement logged here (after time sync) so esphome logs captures it */
    ESP_LOGD("eebus", "SPINE local: device=EnergyManagementSystem entity=CEM(id=1) heartbeat=%us",
             kHeartbeatTimeoutSeconds);
    ESP_LOGD("eebus", "SPINE local use-case: actor=EnergyGuard(9) useCase=limitationOfPowerConsumption(14) [EG/LPC]");
    ESP_LOGD("eebus", "SPINE local use-case: actor=MonitoringAppliance(19) useCase=monitoringOfPowerConsumption(25) [MA/MPC]");
    EEBUS_SERVICE_START(service_);
    pairing_state_ = "Suche WP via mDNS...";
    ESP_LOGI(TAG, "EEBus WP service started after time sync");
  }
}

}  // namespace eebus_wp
}  // namespace esphome
