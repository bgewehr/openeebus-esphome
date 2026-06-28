/*
 * Copyright 2025 bgewehr (bg-hems branch)
 * Licensed under the Apache License, Version 2.0
 */
/**
 * @file eebus_lpc.h
 * @brief ESPHome component wrapping the openeebus LPC CS use case.
 *
 * Lifecycle:
 *   1. Boot: load or generate X.509 cert (NVS). Expose local SKI as text sensor.
 *   2. SHIP service starts, announces via mDNS.
 *   3. CLS-Steuerbox connects → on_remote_ski_connected fires.
 *      - If remote_ski configured: auto-trust that SKI only.
 *      - If empty: pairing mode — expose pending SKI in web UI,
 *        user confirms via button → register_remote_ski(ski, true).
 *   4. Handshake completes (kSmeStateApproved / kDataExchange).
 *   5. LPC limits arrive via CsLpListenerInterface → triggers fire.
 *   6. Heartbeat watchdog: apply failsafe if heartbeat lost > 60 s.
 */

#pragma once

#include <string>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/log.h"
#include "esphome/core/preferences.h"

extern "C" {
#include "src/service/api/eebus_service_interface.h"
#include "src/service/api/eebus_service_config.h"
#include "src/service/api/service_reader_interface.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/model/types.h"
#include "src/use_case/api/cs_lp_listener_interface.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/model/scaled_value.h"
}

namespace esphome {
namespace eebus_lpc {

static const char* const TAG = "eebus_lpc";

class EebusLpcComponent;

/* -------------------------------------------------------------------------
 * Triggers
 * ---------------------------------------------------------------------- */

class LimitActiveTrigger : public Trigger<float> {
 public:
  explicit LimitActiveTrigger(EebusLpcComponent* parent);
};

class LimitClearedTrigger : public Trigger<> {
 public:
  explicit LimitClearedTrigger(EebusLpcComponent* parent);
};

class PairingRequestTrigger : public Trigger<std::string> {
 public:
  explicit PairingRequestTrigger(EebusLpcComponent* parent);
};

/* -------------------------------------------------------------------------
 * Main component
 * ---------------------------------------------------------------------- */

class EebusLpcComponent : public Component {
 public:
  /* ESPHome lifecycle */
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  void dump_config() override;

  /* -----------------------------------------------------------------------
   * Configuration setters
   * -------------------------------------------------------------------- */
  void set_ship_port(uint16_t port)           { ship_port_        = port; }
  void set_remote_ski(const std::string& ski) { remote_ski_       = ski; }
  void set_device_brand(const std::string& b) { device_brand_     = b; }
  void set_device_type(const std::string& t)  { device_type_      = t; }
  void set_device_model(const std::string& m) { device_model_     = m; }
  void set_failsafe_limit_w(float w)          { failsafe_limit_w_ = w; }

  /* -----------------------------------------------------------------------
   * Trigger registration
   * -------------------------------------------------------------------- */
  void add_on_limit_active_trigger(LimitActiveTrigger* t)     { limit_active_triggers_.push_back(t); }
  void add_on_limit_cleared_trigger(LimitClearedTrigger* t)   { limit_cleared_triggers_.push_back(t); }
  void add_on_pairing_request_trigger(PairingRequestTrigger* t) { pairing_request_triggers_.push_back(t); }

  /* -----------------------------------------------------------------------
   * Public API — callable from YAML lambdas and buttons
   * -------------------------------------------------------------------- */

  /** Open a timed pairing window (must be called explicitly before any new device can pair) */
  void enter_pairing_mode();

  /** Accept pairing with the currently pending SKI */
  void accept_pairing();

  /** Reject / cancel pairing with the currently pending SKI */
  void reject_pairing();

  /** Remove a previously trusted SKI and re-open pairing window */
  void forget_pairing(const std::string& ski);

  /* -----------------------------------------------------------------------
   * State accessors (for sensors / text_sensors in YAML)
   * -------------------------------------------------------------------- */
  bool        is_limit_active()      const { return limit_active_; }
  float       current_limit_w()      const { return current_limit_w_; }
  float       failsafe_limit_w()     const { return failsafe_limit_w_; }
  std::string local_ski()            const { return local_ski_; }
  std::string paired_remote_ski()    const { return paired_remote_ski_; }
  std::string pending_remote_ski()   const { return pending_remote_ski_; }
  std::string pairing_state()        const { return pairing_state_str_; }
  bool        is_paired()            const { return !paired_remote_ski_.empty(); }
  bool        has_pending_pairing()  const { return !pending_remote_ski_.empty(); }
  bool        is_pairing_mode()      const { return pairing_mode_active_; }

  /* -----------------------------------------------------------------------
   * Callbacks from openeebus C layer (called by vtable functions below)
   * -------------------------------------------------------------------- */
  void on_remote_ski_connected(const char* ski);
  void on_remote_ski_disconnected(const char* ski);
  void on_ship_state_update(const char* ski, SmeState state);
  void on_ship_id_update(const char* ski, const char* ship_id);
  bool is_waiting_for_trust_allowed(const char* ski);

  void on_power_limit_receive(float limit_w, bool is_active);
  void on_failsafe_limit_receive(float limit_w);
  void on_heartbeat_receive(uint64_t counter);

 protected:
  bool load_or_generate_cert_();
  bool store_cert_nvs_(const uint8_t* cert, size_t cert_len,
                       const uint8_t* key,  size_t key_len);
  bool load_cert_nvs_(uint8_t** cert, size_t* cert_len,
                      uint8_t** key,  size_t* key_len);
  bool start_eebus_service_(const uint8_t* cert_der, size_t cert_len,
                             const uint8_t* key_der,  size_t key_len);

  void save_trusted_ski_(const std::string& ski);
  std::string load_trusted_ski_();
  void update_pairing_state_(const std::string& state);

  /* Config */
  uint16_t    ship_port_        {4712};
  std::string remote_ski_       {};     /* configured in YAML — empty = pairing mode */
  std::string device_brand_     {"DIY"};
  std::string device_type_      {"HEMS"};
  std::string device_model_     {"ESP32-HEMS-14a"};
  float       failsafe_limit_w_ {4200.0f};

  /* Runtime state */
  std::string local_ski_         {};
  std::string paired_remote_ski_ {};    /* trusted, data exchange active */
  std::string pending_remote_ski_{};    /* connected but not yet trusted */
  std::string pairing_state_str_ {"Nicht verbunden"};

  bool        limit_active_        {false};
  float       current_limit_w_     {0.0f};
  bool        heartbeat_lost_      {false};
  uint32_t    last_heartbeat_ms_   {0};
  bool        pairing_mode_active_ {false}; /* true only while explicit pairing window is open */
  uint32_t    pairing_deadline_ms_ {0};     /* absolute millis() deadline for pairing window */

  /* openeebus handles */
  EebusServiceObject*  service_       {nullptr};
  CsLpUseCaseObject*   cs_lpc_        {nullptr};
  EntityLocalObject*   local_entity_  {nullptr};  /* added: needed by start_eebus_service_ */

  /* ServiceReader vtable storage (C struct, must outlive the service) */
  struct ServiceReaderWrapper {
    ServiceReaderObject obj;   /* must be first */
    EebusLpcComponent*  self;
  } service_reader_ {};

  /* Trigger lists */
  std::vector<LimitActiveTrigger*>    limit_active_triggers_;
  std::vector<LimitClearedTrigger*>   limit_cleared_triggers_;
  std::vector<PairingRequestTrigger*> pairing_request_triggers_;

 public:  /* public so free C-linkage vtable functions can reinterpret_cast */
  /* -----------------------------------------------------------------------
   * C-compatible vtable objects — wrap back to this C++ instance.
   * Must be layout-compatible with the C struct (obj is first member).
   * -------------------------------------------------------------------- */
  struct LpcListener {
    CsLpListenerObject obj;   /* must be first */
    EebusLpcComponent* self;
  } lpc_listener_{};
};

/* -------------------------------------------------------------------------
 * C vtable implementations — LPC listener
 * ---------------------------------------------------------------------- */

extern "C" {

static void LpcListenerDestruct(CsLpListenerObject*) {}

static void LpcListenerOnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue*  power_limit,
    const DurationType* /*duration*/,
    bool                is_active)
{
  auto* l = reinterpret_cast<EebusLpcComponent::LpcListener*>(self);
  float w = (float)power_limit->value * powf(10.0f, (float)power_limit->scale);
  l->self->on_power_limit_receive(w, is_active);
}

static void LpcListenerOnFailsafePowerLimitReceive(
    CsLpListenerObject* self, const ScaledValue* power_limit)
{
  auto* l = reinterpret_cast<EebusLpcComponent::LpcListener*>(self);
  float w = (float)power_limit->value * powf(10.0f, (float)power_limit->scale);
  l->self->on_failsafe_limit_receive(w);
}

static void LpcListenerOnFailsafeDurationReceive(
    CsLpListenerObject*, const DurationType*) {}

static void LpcListenerOnHeartbeatReceive(
    CsLpListenerObject* self, uint64_t counter)
{
  reinterpret_cast<EebusLpcComponent::LpcListener*>(self)->self->on_heartbeat_receive(counter);
}

static const CsLpListenerInterface kLpcListenerMethods = {
  .destruct                        = LpcListenerDestruct,
  .on_power_limit_receive          = LpcListenerOnPowerLimitReceive,
  .on_failsafe_power_limit_receive = LpcListenerOnFailsafePowerLimitReceive,
  .on_failsafe_duration_receive    = LpcListenerOnFailsafeDurationReceive,
  .on_heartbeat_receive            = LpcListenerOnHeartbeatReceive,
};

}  // extern "C"

/* -------------------------------------------------------------------------
 * Trigger constructors
 * ---------------------------------------------------------------------- */

inline LimitActiveTrigger::LimitActiveTrigger(EebusLpcComponent* p)
  { p->add_on_limit_active_trigger(this); }

inline LimitClearedTrigger::LimitClearedTrigger(EebusLpcComponent* p)
  { p->add_on_limit_cleared_trigger(this); }

inline PairingRequestTrigger::PairingRequestTrigger(EebusLpcComponent* p)
  { p->add_on_pairing_request_trigger(this); }

}  // namespace eebus_lpc
}  // namespace esphome
