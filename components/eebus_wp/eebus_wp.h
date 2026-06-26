/*
 * Copyright 2025 bgewehr (bg-hems branch)
 * Licensed under the Apache License, Version 2.0
 */
/**
 * @file eebus_wp.h
 * @brief ESPHome component: EEBus EG/LPC sender to Bosch K40RF gateway.
 *
 * The HEMS acts as Energy Guard (EG/CEM) and sends LPC power limits
 * to the Bosch Compress 5800i via its K40RF EEBus Gateway (CS role).
 *
 * The K40RF is discovered automatically via mDNS. After pairing its SKI
 * appears in the web UI (same pairing flow as eebus_lpc component).
 *
 * API for YAML lambdas:
 *   id(hems_wp).set_limit(watts)     — send LPC limit now
 *   id(hems_wp).clear_limit()        — remove limit (full power)
 *   id(hems_wp).current_power_w()    — last MPC reading from WP (W)
 *   id(hems_wp).is_connected()       — K40RF connection state
 *   id(hems_wp).remote_ski()         — K40RF SKI
 */

#pragma once

#include <string>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/log.h"

extern "C" {
#include "src/service/api/eebus_service_interface.h"
#include "src/service/api/eebus_service_config.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/model/types.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/model/scaled_value.h"
#include "src/spine/api/entity_remote_interface.h"
}

namespace esphome {
namespace eebus_wp {

static const char* const TAG = "eebus_wp";

class EebusWpComponent;

/* -------------------------------------------------------------------------
 * Triggers
 * ---------------------------------------------------------------------- */

class WpConnectedTrigger : public Trigger<> {
 public: explicit WpConnectedTrigger(EebusWpComponent* p);
};
class WpDisconnectedTrigger : public Trigger<> {
 public: explicit WpDisconnectedTrigger(EebusWpComponent* p);
};
class WpPowerReadingTrigger : public Trigger<float> {
 public: explicit WpPowerReadingTrigger(EebusWpComponent* p);
};

/* -------------------------------------------------------------------------
 * Main component
 * ---------------------------------------------------------------------- */

class EebusWpComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  void dump_config() override;

  /* Config setters */
  void set_ship_port(uint16_t p)              { ship_port_          = p; }
  void set_remote_ski(const std::string& s)   { remote_ski_         = s; }
  void set_device_brand(const std::string& b) { device_brand_       = b; }
  void set_device_type(const std::string& t)  { device_type_        = t; }
  void set_device_model(const std::string& m) { device_model_       = m; }
  void set_failsafe_limit_w(float w)          { failsafe_limit_w_   = w; }
  void set_failsafe_duration_s(uint32_t s)    { failsafe_duration_s_= s; }

  /* Trigger registration */
  void add_on_wp_connected_trigger(WpConnectedTrigger* t)    { connected_triggers_.push_back(t); }
  void add_on_wp_disconnected_trigger(WpDisconnectedTrigger* t) { disconnected_triggers_.push_back(t); }
  void add_on_power_reading_trigger(WpPowerReadingTrigger* t)   { power_triggers_.push_back(t); }

  /* -----------------------------------------------------------------------
   * Public API — callable from YAML lambdas
   * -------------------------------------------------------------------- */

  /**
   * @brief Send LPC power limit to K40RF/WP.
   * @param watts  Maximum allowed consumption in watts (≥ 4200 W per §14a).
   *               Pass 0 to disable limit (full power).
   */
  void set_limit(float watts);

  /**
   * @brief Remove active LPC limit — WP returns to full power.
   */
  void clear_limit() { set_limit(0.0f); }

  /* State accessors */
  bool        is_connected()     const { return connected_; }
  float       current_power_w()  const { return current_power_w_; }
  float       active_limit_w()   const { return active_limit_w_; }
  std::string remote_ski()       const { return remote_ski_; }
  std::string pairing_state()    const { return pairing_state_; }

  /* Called from C vtable */
  void on_entity_connect(const EntityAddressType* addr);
  void on_entity_disconnect(const EntityAddressType* addr);
  void on_power_limit_ack(float watts, bool active);
  void on_power_reading(float watts);

 protected:
  bool load_or_generate_cert_();
  bool store_cert_nvs_(const uint8_t* c, size_t cl, const uint8_t* k, size_t kl);
  bool load_cert_nvs_(uint8_t** c, size_t* cl, uint8_t** k, size_t* kl);
  bool start_eebus_service_(const uint8_t* cert, size_t cl, const uint8_t* key, size_t kl);

  /* Config */
  uint16_t    ship_port_          {4712};
  std::string remote_ski_         {};
  std::string device_brand_       {"DIY"};
  std::string device_type_        {"HEMS"};
  std::string device_model_       {"ESP32-HEMS-14a"};
  float       failsafe_limit_w_   {4200.0f};
  uint32_t    failsafe_duration_s_{7200};

  /* Runtime */
  bool        connected_          {false};
  float       current_power_w_    {0.0f};
  float       active_limit_w_     {0.0f};
  std::string pairing_state_      {"Nicht verbunden"};

  EntityAddressType remote_entity_addr_{};
  bool              have_remote_entity_{false};

  /* openeebus */
  EebusServiceObject* service_  {nullptr};
  EgLpUseCaseObject*  eg_lpc_   {nullptr};

  /* Trigger lists */
  std::vector<WpConnectedTrigger*>    connected_triggers_;
  std::vector<WpDisconnectedTrigger*> disconnected_triggers_;
  std::vector<WpPowerReadingTrigger*> power_triggers_;

  /* C-vtable wrapper */
  struct EgListener {
    EgLpListenerObject obj;   /* must be first */
    EebusWpComponent*  self;
  } eg_listener_{};
};

/* -------------------------------------------------------------------------
 * C vtable for EgLpListenerInterface
 * ---------------------------------------------------------------------- */

extern "C" {

static void EgL_Destruct(EgLpListenerObject*) {}

static void EgL_OnRemoteEntityConnect(EgLpListenerObject* o, const EntityAddressType* addr) {
  reinterpret_cast<EebusWpComponent::EgListener*>(o)->self->on_entity_connect(addr);
}
static void EgL_OnRemoteEntityDisconnect(EgLpListenerObject* o, const EntityAddressType* addr) {
  reinterpret_cast<EebusWpComponent::EgListener*>(o)->self->on_entity_disconnect(addr);
}
static void EgL_OnPowerLimitReceive(
    EgLpListenerObject* o, const EntityAddressType* /*addr*/,
    const ScaledValue* limit, const DurationType* /*dur*/, bool active)
{
  float w = (float)limit->value * powf(10.0f, (float)limit->scale);
  reinterpret_cast<EebusWpComponent::EgListener*>(o)->self->on_power_limit_ack(w, active);
}
static void EgL_OnFailsafePowerLimitReceive(EgLpListenerObject*, const EntityAddressType*, const ScaledValue*) {}
static void EgL_OnFailsafeDurationReceive(EgLpListenerObject*, const EntityAddressType*, const DurationType*) {}
static void EgL_OnHeartbeatReceive(EgLpListenerObject*, const EntityAddressType*, uint64_t) {}

static const EgLpListenerInterface kEgListenerMethods = {
  .destruct                      = EgL_Destruct,
  .on_remote_entity_connect      = EgL_OnRemoteEntityConnect,
  .on_remote_entity_disconnect   = EgL_OnRemoteEntityDisconnect,
  .on_power_limit_receive        = EgL_OnPowerLimitReceive,
  .on_failsafe_power_limit_receive = EgL_OnFailsafePowerLimitReceive,
  .on_failsafe_duration_receive  = EgL_OnFailsafeDurationReceive,
  .on_heartbeat_receive          = EgL_OnHeartbeatReceive,
};

}  // extern "C"

/* Trigger constructors */
inline WpConnectedTrigger::WpConnectedTrigger(EebusWpComponent* p)
  { p->add_on_wp_connected_trigger(this); }
inline WpDisconnectedTrigger::WpDisconnectedTrigger(EebusWpComponent* p)
  { p->add_on_wp_disconnected_trigger(this); }
inline WpPowerReadingTrigger::WpPowerReadingTrigger(EebusWpComponent* p)
  { p->add_on_power_reading_trigger(this); }

}  // namespace eebus_wp
}  // namespace esphome
