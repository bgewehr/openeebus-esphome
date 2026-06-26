"""ESPHome external component: eebus_lpc

EEBus SHIP/SPINE LPC CS actor for §14a EnWG.

Pairing flow:
  1. First boot: certificate generated, local SKI shown in web UI.
  2. CLS-Steuerbox connects → pending SKI appears in web UI.
  3. User presses "Pairing akzeptieren" button in web UI.
  4. SKI persisted to NVS — survives reboot.
  5. LPC limits received → on_limit_active trigger fires.
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ["network", "esp32"]
CODEOWNERS   = ["@bgewehr"]
MULTI_CONF   = False

eebus_lpc_ns       = cg.esphome_ns.namespace("eebus_lpc")
EebusLpcComponent  = eebus_lpc_ns.class_("EebusLpcComponent", cg.Component)

LimitActiveTrigger   = eebus_lpc_ns.class_("LimitActiveTrigger",   automation.Trigger.template(cg.float_))
LimitClearedTrigger  = eebus_lpc_ns.class_("LimitClearedTrigger",  automation.Trigger.template())
PairingRequestTrigger= eebus_lpc_ns.class_("PairingRequestTrigger", automation.Trigger.template(cg.std_string))

AcceptPairingAction = eebus_lpc_ns.class_("AcceptPairingAction", automation.Action)
RejectPairingAction = eebus_lpc_ns.class_("RejectPairingAction", automation.Action)

CONF_REMOTE_SKI         = "remote_ski"
CONF_SHIP_PORT          = "ship_port"
CONF_DEVICE_BRAND       = "device_brand"
CONF_DEVICE_TYPE        = "device_type"
CONF_DEVICE_MODEL       = "device_model"
CONF_FAILSAFE_LIMIT     = "failsafe_limit_w"
CONF_ON_LIMIT_ACTIVE    = "on_limit_active"
CONF_ON_LIMIT_CLEARED   = "on_limit_cleared"
CONF_ON_PAIRING_REQUEST = "on_pairing_request"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EebusLpcComponent),
    cv.Optional(CONF_SHIP_PORT,      default=4712):          cv.port,
    cv.Optional(CONF_REMOTE_SKI,     default=""):            cv.string,
    cv.Optional(CONF_DEVICE_BRAND,   default="DIY"):         cv.string_strict,
    cv.Optional(CONF_DEVICE_TYPE,    default="HEMS"):        cv.string_strict,
    cv.Optional(CONF_DEVICE_MODEL,   default="ESP32-HEMS-14a"): cv.string_strict,
    cv.Optional(CONF_FAILSAFE_LIMIT, default=4200.0):        cv.positive_float,
    cv.Optional(CONF_ON_LIMIT_ACTIVE): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LimitActiveTrigger),
    }),
    cv.Optional(CONF_ON_LIMIT_CLEARED): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LimitClearedTrigger),
    }),
    cv.Optional(CONF_ON_PAIRING_REQUEST): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PairingRequestTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_ship_port(config[CONF_SHIP_PORT]))
    cg.add(var.set_remote_ski(config[CONF_REMOTE_SKI]))
    cg.add(var.set_device_brand(config[CONF_DEVICE_BRAND]))
    cg.add(var.set_device_type(config[CONF_DEVICE_TYPE]))
    cg.add(var.set_device_model(config[CONF_DEVICE_MODEL]))
    cg.add(var.set_failsafe_limit_w(config[CONF_FAILSAFE_LIMIT]))

    for conf in config.get(CONF_ON_LIMIT_ACTIVE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.float_, "x")], conf)

    for conf in config.get(CONF_ON_LIMIT_CLEARED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_PAIRING_REQUEST, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)
