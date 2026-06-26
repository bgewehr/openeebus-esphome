"""ESPHome external component: eebus_lpc

EEBus SHIP/SPINE LPC CS actor for §14a EnWG.

Pairing flow:
  1. First boot: certificate generated, local SKI shown in web UI.
  2. CLS-Steuerbox connects → pending SKI appears in web UI.
  3. User presses "Pairing akzeptieren" button in web UI.
  4. SKI persisted to NVS — survives reboot.
  5. LPC limits received → on_limit_active trigger fires.
"""

import os
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


def _generate_unity_build(component_dir, repo_root):
    """Generate openeebus_unity.c using include-path-relative paths.

    The compiler receives two -I flags:
      1. repo_root/          (b72b2cfd/)
      2. repo_root/openeebus (b72b2cfd/openeebus/)

    So openeebus sources are included as:
      #include "src/common/..."        (resolved via -I openeebus/)
    And ESP32 port sources as:
      #include "port/esp32/websocket/" (resolved via -I repo_root/)

    This makes the generated unity file completely path-independent —
    the same content works on any machine because the paths are relative
    to the stable -I include roots, not to the file's location on disk.
    """
    openeebus_src = os.path.join(repo_root, "openeebus", "src")
    port_ws       = os.path.join(repo_root, "port", "esp32", "websocket")

    EXCLUDE = {
        "debug.c",                  # libwebsockets → replaced inline
        "tls_certificate.c",        # OpenSSL → _mbedtls.c used
        "eebus_mutex.c",            # pthread → _freertos.c used
        "eebus_thread.c",           # pthread → _freertos.c used
        "eebus_queue.c",            # pthread → _freertos.c used
        "websocket.c",              # libwebsockets → port/esp32/ replaces all
        "websocket_client.c",
        "websocket_server.c",
        "websocket_creator.c",
        "websocket_server_creator.c",
        "websocket_client_creator.c",
        "websocket_debug.c",
    }
    EXCLUDE_PATTERNS = (
        "eebus_timer_linux", "eebus_timer_windows", "eebus_timer_apple",
        "http_server", "openssl_util", "applink", "_bonjour", "eebus_cli",
    )

    def should_exclude(fname):
        return fname in EXCLUDE or any(p in fname for p in EXCLUDE_PATTERNS)

    # Collect openeebus C sources; express paths relative to openeebus_root
    # so the compiler finds them via the second -I flag.
    oe_includes = []
    for root, _, files in os.walk(openeebus_src):
        for fname in sorted(files):
            if fname.endswith(".c") and not should_exclude(fname):
                rel = os.path.relpath(
                    os.path.join(root, fname), os.path.join(repo_root, "openeebus")
                ).replace("\\", "/")
                oe_includes.append(rel)
    oe_includes.sort()

    # ESP32 port sources; express relative to repo_root
    # so the compiler finds them via the first -I flag.
    port_includes = sorted(
        "port/esp32/websocket/" + f
        for f in os.listdir(port_ws)
        if f.endswith(".c")
    )

    lines = [
        "// openeebus_unity.c — generated by eebus_lpc/__init__.py",
        "// Overwritten on every `esphome compile`. Do not edit manually.",
        "//",
        "// Paths are relative to the -I include roots, not to this file.",
        "// This file is portable: no machine-specific absolute paths.",
        "",
        "#define EEBUS_PLATFORM_FREERTOS 1",
        "#define EEBUS_PLATFORM_ESP32    1",
        "",
        "// ESP32 replacement for src/common/debug.c",
        "// (original requires libwebsockets; we use esp_log instead)",
        "#include <stdarg.h>",
        "#include <stddef.h>",
        '#include "esp_log.h"',
        'static const char *EEBUS_DBG_TAG_ = "openeebus";',
        "void DebugPrintf(const char *format, ...) {",
        "  va_list args; va_start(args, format);",
        "  esp_log_writev(ESP_LOG_DEBUG, EEBUS_DBG_TAG_, format, args);",
        "  va_end(args);",
        "}",
        "void DebugHexdump(void *data, size_t data_size) {",
        "  ESP_LOG_BUFFER_HEXDUMP(EEBUS_DBG_TAG_, data, data_size, ESP_LOG_DEBUG);",
        "}",
        "",
        f"// openeebus core ({len(oe_includes)} files, resolved via -I openeebus/)",
    ]
    for inc in oe_includes:
        lines.append(f'#include "{inc}"')
    lines += [
        "",
        f"// ESP32 WebSocket port ({len(port_includes)} files, resolved via -I repo_root/)",
    ]
    for inc in port_includes:
        lines.append(f'#include "{inc}"')

    unity_path = os.path.join(component_dir, "openeebus_unity.c")
    with open(unity_path, "w", encoding="utf-8") as fh:
        fh.write("\n".join(lines) + "\n")


async def to_code(config):
    component_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root     = os.path.dirname(os.path.dirname(component_dir))

    # Generate openeebus_unity.c with include-path-relative paths.
    # Runs before ESPHome copies source files to build/src/, so the
    # generated content will be used in the build.
    _generate_unity_build(component_dir, repo_root)

    # Add include search paths (forward slashes — required by Xtensa GCC on Windows).
    for path in (repo_root, os.path.join(repo_root, "openeebus")):
        cg.add_build_flag("-I" + path.replace("\\", "/"))

    # cJSON is included as <cjson/cJSON.h>. ESP-IDF ships it at
    # components/json/cJSON/ — add the parent so the include resolves.
    import glob as _glob
    idf_cjson = _glob.glob(os.path.join(
        os.path.expanduser("~"), ".platformio", "packages", "framework-espidf*",
        "components", "json", "cJSON",
    ))
    if idf_cjson:
        cg.add_build_flag("-I" + os.path.dirname(idf_cjson[0]).replace("\\", "/"))
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
