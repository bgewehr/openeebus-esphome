"""ESPHome external component: eebus_wp

EEBus SHIP/SPINE LPC Energy Guard (EG) actor — sends power limits to the
Bosch Compress 5800i via the K40RF EEBus Gateway.

The HEMS acts as CEM/EG, the K40RF acts as CS (Controllable System).
The K40RF is discovered automatically via mDNS (_ship._tcp).

Use cases supported by Bosch K40RF:
  LPC  — Limitation of Power Consumption  (we send limits)
  MPC  — Monitoring of Power Consumption  (we read actual power)
  OHPCF — PV optimisation (via LPC + limit scheduling)

Example YAML:
    eebus_wp:
      id: hems_wp
      remote_ski: "aabbcc..."   # SKI of K40RF — from web UI after pairing
      on_wp_connected:
        - logger.log: "K40RF connected"
      on_wp_disconnected:
        - logger.log: "K40RF disconnected"
"""

import os
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import socket
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ["network", "esp32"]
CODEOWNERS   = ["@bgewehr"]
MULTI_CONF   = False

eebus_wp_ns       = cg.esphome_ns.namespace("eebus_wp")
EebusWpComponent  = eebus_wp_ns.class_("EebusWpComponent", cg.Component)

WpConnectedTrigger    = eebus_wp_ns.class_("WpConnectedTrigger",    automation.Trigger.template())
WpDisconnectedTrigger = eebus_wp_ns.class_("WpDisconnectedTrigger", automation.Trigger.template())
WpPowerReadingTrigger = eebus_wp_ns.class_("WpPowerReadingTrigger", automation.Trigger.template(cg.float_))

CONF_REMOTE_SKI          = "remote_ski"
CONF_SHIP_PORT           = "ship_port"
CONF_DEVICE_BRAND        = "device_brand"
CONF_DEVICE_TYPE         = "device_type"
CONF_DEVICE_MODEL        = "device_model"
CONF_FAILSAFE_LIMIT_W    = "failsafe_limit_w"
CONF_FAILSAFE_DURATION_S = "failsafe_duration_s"
CONF_ON_WP_CONNECTED     = "on_wp_connected"
CONF_ON_WP_DISCONNECTED  = "on_wp_disconnected"
CONF_ON_POWER_READING    = "on_power_reading"

def _consume_eebus_wp_sockets(config):
    # httpd_ssl instance: 1 HTTPS listen + 2 active WS connections + 1 ctrl_port = 4 sockets
    socket.consume_sockets(1, "eebus_wp", socket.SocketType.TCP_LISTEN)(config)
    socket.consume_sockets(3, "eebus_wp")(config)
    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(EebusWpComponent),
        cv.Optional(CONF_SHIP_PORT,           default=4712):    cv.port,
        cv.Optional(CONF_REMOTE_SKI,          default=""):      cv.string,
        cv.Optional(CONF_DEVICE_BRAND,        default="DIY"):   cv.string_strict,
        cv.Optional(CONF_DEVICE_TYPE,         default="HEMS"):  cv.string_strict,
        cv.Optional(CONF_DEVICE_MODEL,        default="ESP32-HEMS-14a"): cv.string_strict,
        cv.Optional(CONF_FAILSAFE_LIMIT_W,    default=4200.0):  cv.positive_float,
        cv.Optional(CONF_FAILSAFE_DURATION_S, default=7200):    cv.positive_int,  # 2h default
        cv.Optional(CONF_ON_WP_CONNECTED): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(WpConnectedTrigger),
        }),
        cv.Optional(CONF_ON_WP_DISCONNECTED): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(WpDisconnectedTrigger),
        }),
        cv.Optional(CONF_ON_POWER_READING): automation.validate_automation({
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(WpPowerReadingTrigger),
        }),
    }).extend(cv.COMPONENT_SCHEMA),
    _consume_eebus_wp_sockets,
)


async def to_code(config):
    repo_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    openeebus_root = os.path.join(repo_root, "openeebus")
    for path in (repo_root, openeebus_root):
        cg.add_build_flag("-I" + path.replace("\\", "/"))
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_ship_port(config[CONF_SHIP_PORT]))
    cg.add(var.set_remote_ski(config[CONF_REMOTE_SKI]))
    cg.add(var.set_device_brand(config[CONF_DEVICE_BRAND]))
    cg.add(var.set_device_type(config[CONF_DEVICE_TYPE]))
    cg.add(var.set_device_model(config[CONF_DEVICE_MODEL]))
    cg.add(var.set_failsafe_limit_w(config[CONF_FAILSAFE_LIMIT_W]))
    cg.add(var.set_failsafe_duration_s(config[CONF_FAILSAFE_DURATION_S]))

    for conf in config.get(CONF_ON_WP_CONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_WP_DISCONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_POWER_READING, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.float_, "x")], conf)
