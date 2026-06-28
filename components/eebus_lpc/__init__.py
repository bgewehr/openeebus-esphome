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
from esphome.components import socket
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

def _consume_eebus_lpc_sockets(config):
    # httpd_ssl instance: 1 HTTPS listen + 2 active WS connections + 1 ctrl_port = 4 sockets
    socket.consume_sockets(1, "eebus_lpc", socket.SocketType.TCP_LISTEN)(config)
    socket.consume_sockets(3, "eebus_lpc")(config)
    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema({
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
    }).extend(cv.COMPONENT_SCHEMA),
    _consume_eebus_lpc_sockets,
)


def _generate_unity_build(component_dir, repo_root):
    """Generate per-source wrapper .c files for the openeebus library.

    Each wrapper includes exactly one openeebus C source file, so every
    openeebus source compiles as its own translation unit. This avoids
    symbol collisions that occur with a single-file unity build (the
    openeebus code uses file-scoped static function names that collide
    across translation units).

    All wrappers are placed in component_dir alongside this __init__.py.
    ESPHome's ComponentManifest scans that directory for .c files and
    copies them to build/src/esphome/components/eebus_lpc/.
    The generated src/CMakeLists.txt uses GLOB_RECURSE, so they are
    compiled automatically.

    Include paths are relative to the -I search roots:
      -I repo_root/openeebus/  ->  #include "src/common/foo.c"
      -I repo_root/            ->  #include "port/esp32/websocket/bar.c"
    """
    openeebus_src = os.path.join(repo_root, "openeebus", "src")
    port_ws       = os.path.join(repo_root, "port", "esp32", "websocket")

    EXCLUDE = {
        "debug.c",                  # libwebsockets → replaced inline (see below)
        "tls_certificate.c",        # OpenSSL      → _mbedtls.c used
        "eebus_mutex.c",            # pthread       → _freertos.c used
        "eebus_thread.c",           # pthread       → _freertos.c used
        "eebus_queue.c",            # pthread       → _freertos.c used
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

    # Remove previously generated wrapper files (prefix oe__)
    for f in os.listdir(component_dir):
        if f.startswith("oe__") and f.endswith(".c"):
            os.remove(os.path.join(component_dir, f))

    written = []

    # debug.c replacement — single wrapper with inline ESP32 implementation
    debug_path = os.path.join(component_dir, "oe__debug_esp32.c")
    with open(debug_path, "w", encoding="utf-8") as fh:
        fh.write(
            "// ESP32 replacement for openeebus src/common/debug.c\n"
            "// (original requires libwebsockets)\n"
            "#include <stdarg.h>\n"
            "#include <stddef.h>\n"
            '#include "esp_log.h"\n'
            "#define EEBUS_PLATFORM_FREERTOS 1\n"
            "#define EEBUS_PLATFORM_ESP32    1\n"
            'static const char *EEBUS_DBG_TAG_ = "openeebus";\n'
            "void DebugPrintf(const char *format, ...) {\n"
            "  va_list args; va_start(args, format);\n"
            "  esp_log_writev(ESP_LOG_DEBUG, EEBUS_DBG_TAG_, format, args);\n"
            "  va_end(args);\n"
            "}\n"
            "void DebugHexdump(void *data, size_t data_size) {\n"
            "  ESP_LOG_BUFFER_HEXDUMP(EEBUS_DBG_TAG_, data, data_size, ESP_LOG_DEBUG);\n"
            "}\n"
        )
    written.append("oe__debug_esp32.c")

    # One wrapper per openeebus C source
    for root, _, files in os.walk(openeebus_src):
        for fname in sorted(files):
            if not fname.endswith(".c") or should_exclude(fname):
                continue
            src_path = os.path.join(root, fname)
            rel = os.path.relpath(src_path, os.path.join(repo_root, "openeebus")).replace("\\", "/")
            # Unique filename: flatten path, prefix oe__
            safe_name = "oe__" + rel.replace("/", "__").replace(".", "_") + ".c"
            wrapper = os.path.join(component_dir, safe_name)
            with open(wrapper, "w", encoding="utf-8") as fh:
                fh.write(
                    f"// openeebus wrapper: {rel}\n"
                    "#define EEBUS_PLATFORM_FREERTOS 1\n"
                    "#define EEBUS_PLATFORM_ESP32    1\n"
                    f'#include "{rel}"\n'
                )
            written.append(safe_name)

    # One wrapper per ESP32 port source
    for fname in sorted(os.listdir(port_ws)):
        if not fname.endswith(".c"):
            continue
        rel = f"port/esp32/websocket/{fname}"
        safe_name = "oe__" + rel.replace("/", "__").replace(".", "_") + ".c"
        wrapper = os.path.join(component_dir, safe_name)
        with open(wrapper, "w", encoding="utf-8") as fh:
            fh.write(
                f"// openeebus ESP32 port wrapper: {fname}\n"
                "#define EEBUS_PLATFORM_FREERTOS 1\n"
                "#define EEBUS_PLATFORM_ESP32    1\n"
                f'#include "{rel}"\n'
            )
        written.append(safe_name)

    return written


async def to_code(config):
    component_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root     = os.path.dirname(os.path.dirname(component_dir))

    # Generate per-source wrapper .c files (oe__*.c) in this directory.
    # Each wrapper includes exactly one openeebus C source so they compile
    # as separate translation units — avoids symbol collisions in unity builds.
    # Runs before ESPHome copies source files to build/src/.
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

    # esp_websocket_client is required by the port/esp32 WebSocket client layer
    # (used by eebus_wp for outbound SHIP connections to the K40RF gateway).
    # In ESP-IDF 5.x it is a managed component — declare it so the IDF
    # Component Manager downloads and includes it in the build.
    from esphome.components.esp32 import (
        add_idf_component,
        add_idf_sdkconfig_option,
        include_builtin_idf_component,
    )
    add_idf_component(name="espressif/esp_websocket_client", ref="1.3.0")

    # The port/esp32 WebSocket server layer uses the ESP-IDF HTTP server's
    # built-in WebSocket support (httpd_ws_frame_t, httpd_ws_send_frame, etc.).
    # Enable it in sdkconfig so the relevant struct fields and APIs are compiled in.
    add_idf_sdkconfig_option("CONFIG_HTTPD_WS_SUPPORT", True)

    # The SHIP server uses TLS (httpd_ssl_config_t, httpd_ssl_start) which lives
    # in esp_https_server — excluded by ESPHome by default.
    include_builtin_idf_component("esp_https_server")
    add_idf_sdkconfig_option("CONFIG_ESP_HTTPS_SERVER_ENABLE", True)

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
