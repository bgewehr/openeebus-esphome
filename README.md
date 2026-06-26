# openeebus-esphome

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![ESPHome](https://img.shields.io/badge/ESPHome-external--component-green)](https://esphome.io/components/external_components)
[![§14a EnWG](https://img.shields.io/badge/§14a-EnWG-orange)](https://www.gesetze-im-internet.de/enwg_2005/__14a.html)

**EEBus SHIP/SPINE external components for ESPHome — §14a EnWG HEMS on ESP32**

`openeebus-esphome` brings EEBus LPC (Limitation of Power Consumption) to
ESP32-based DIY home energy management systems. It enables a single ESP32
Hutschienen device to act as a compliant §14a EnWG HEMS by:

- **receiving** power-limit commands from a Westnetz CLS-Steuerbox (CS role)
- **sending** power-limit commands to a Bosch Compress heat pump via K40RF
  gateway (EG role)
- controlling a Shell Recharge wallbox via Modbus TCP
- exposing a full status and pairing UI via the built-in ESPHome web server

The EEBus protocol implementation is provided by
[NIBEGroup/openeebus](https://github.com/NIBEGroup/openeebus) (Apache 2.0),
integrated as a git submodule. This project contributes the ESP32 WebSocket
adapter layer and the ESPHome component wrappers — the protocol core is used
unmodified.

> **Status:** Implementation complete. Runtime validation against a real
> Westnetz CLS-Steuerbox is pending (iMSys installation in progress).
> The relay-contact fallback path (DI1 GPIO) is fully functional and
> regulatorily sufficient until 31 December 2028 per Westnetz DRW-O-AN §6.2.1.

---

## Architecture

```
Westnetz CLS-Steuerbox (EG)
    │  EEBus LPC over SHIP/SPINE/TLS  (port 4712)
    ▼
ESP32 HEMS  ←— this project
    │  EEBus LPC over SHIP/SPINE/TLS  (port 4713)
    ▼
Bosch K40RF EEBus Gateway (CS)
    │  EMS-BUS internal
    ▼
Bosch Compress 5800i heat pump

ESP32 HEMS ——Modbus TCP——► Shell Recharge wallbox
ESP32 HEMS ——DO1/DO2 GPIO► Bosch WP SG-Ready (fallback)
ESP32 HEMS ◄——DI1 GPIO———— CLS-Steuerbox relay (fallback)
```

---

## Requirements

**Hardware**

| Component | Example |
|---|---|
| ESP32 board with Ethernet | Olimex ESP32-POE-ISO, LILYGO T-Internet-POE |
| 8× DI/DO (galv. isolated) | REG Hutschienen PCB |
| Heat pump gateway | Bosch K40RF (Art.-Nr. 7738113771) |
| Wallbox | Shell Recharge (Modbus TCP) |
| VNB interface | Westnetz CLS-Steuerbox (installed by Westnetz) |

> WiFi works but Ethernet is strongly recommended — EEBus requires stable
> TCP connections and low-latency TLS handshakes.

**Software**

- ESPHome ≥ 2024.6 with ESP-IDF framework
- Python ≥ 3.10

---

## Quick start

### 1. Clone with submodule

```bash
git clone --recurse-submodules https://github.com/bgewehr/openeebus-esphome.git
```

### 2. Add to your ESPHome configuration

```yaml
external_components:
  - source: github://bgewehr/openeebus-esphome@main
    components: [eebus_lpc, eebus_wp]
```

### 3. Configure eebus_lpc (receive from CLS-Steuerbox)

```yaml
eebus_lpc:
  id: hems_lpc
  ship_port: 4712
  remote_ski: ""            # Empty = pairing mode; persisted to NVS after first pairing
  failsafe_limit_w: 4200.0  # §14a minimum — applied if heartbeat is lost

  on_limit_active:
    - then:
        - lambda: |-
            float limit_w = x;
            id(hems_wp).set_limit(limit_w - 3900.0f);  // Forward to heat pump
            id(wallbox_ladestrom).set_value(6.0f);      // Wallbox to minimum

  on_limit_cleared:
    - then:
        - lambda: |-
            id(hems_wp).clear_limit();
            id(wallbox_ladestrom).set_value(16.0f);
```

### 4. Configure eebus_wp (send to Bosch K40RF)

```yaml
eebus_wp:
  id: hems_wp
  ship_port: 4713
  remote_ski: ""            # Empty = auto-discover K40RF via mDNS
  failsafe_limit_w: 4200.0
  failsafe_duration_s: 7200

  on_wp_connected:
    - logger.log: "K40RF connected — EEBus heat pump control active"
  on_wp_disconnected:
    - logger.log: "K40RF disconnected — falling back to SG-Ready"
```

See [`examples/hems_14a_example.yaml`](examples/hems_14a_example.yaml) for
the complete configuration including SG-Ready fallback, Modbus TCP wallbox,
and the web server status UI.

---

## Pairing

### CLS-Steuerbox

1. Flash the firmware — the device announces itself via mDNS as `_ship._tcp`
2. The CLS-Steuerbox connects automatically
3. The pending SKI appears in the web UI under **"Pairing SKI (ausstehend)"**
4. Press **"Pairing akzeptieren ✓"** — the SKI is stored in NVS
5. After reboot: automatic re-pairing, no user interaction needed

To find your local SKI (needed if the CLS-Steuerbox requires whitelist entry):
open `http://<device-ip>/` and read **"Eigene SKI (lokal)"**.

### Bosch K40RF

1. The K40RF is discovered automatically via mDNS
2. On the heat pump display: *System Settings → EEBus → Search EEBus devices*
3. Select the ESP32 and confirm
4. The K40RF SKI appears in the web UI under **"K40RF SKI"**

---

## Web UI

The built-in ESPHome web server (`http://<device-ip>/`) shows:

| Entity | Description |
|---|---|
| EEBus Pairing-Status | Current SHIP connection state |
| Eigene SKI (lokal) | This device's EEBus identity |
| Gepaarte Steuerbox-SKI | Trusted CLS-Steuerbox |
| Pairing SKI (ausstehend) | SKI waiting for user confirmation |
| §14a Steuerungskanal | Active control path (EEBus / Relay / None) |
| SG-Ready Modus | Current heat pump operating mode |
| VNB Leistungslimit | Active power limit from VNB (W) |
| K40RF Verbindungsstatus | Heat pump gateway connection |
| WP Ist-Leistung (EEBus) | Actual heat pump consumption (W) |

---

## Memory footprint (ESP32, estimated)

| Component | RAM |
|---|---|
| openeebus SHIP/SPINE core | ~80 KB |
| mbedTLS (×2 sessions) | ~100 KB |
| httpd + WebSocket buffers | ~30 KB |
| FreeRTOS tasks | ~24 KB |
| ESPHome + Ethernet | ~60 KB |
| **Total** | **~294 KB** of 520 KB |

For **ESP32-S3 with PSRAM**: set `CONFIG_EEBUS_USE_PSRAM=y` in
`sdkconfig.defaults` to route openeebus heap allocations to SPIRAM.

---

## Repository structure

```
components/
├── eebus_lpc/      EEBus LPC CS — receives limits from CLS-Steuerbox
└── eebus_wp/       EEBus LPC EG — sends limits to Bosch K40RF

port/esp32/
├── component/      ESP-IDF CMakeLists, PSRAM allocator
└── websocket/      WebSocket server/client (replaces libwebsockets)

examples/
└── hems_14a_example.yaml   Complete §14a HEMS configuration

openeebus/          ← git submodule: NIBEGroup/openeebus (unmodified)
```

---

## Regulatory context (Germany §14a EnWG)

| Control path | Legal basis | Status |
|---|---|---|
| Relay contact DI1 → SG-Ready DO1/DO2 | Westnetz DRW-O-AN §6.2.1 transition period | ✅ valid until 31.12.2028 |
| EEBus digital (this project) | Westnetz DRW-O-AN §6.1 preferred solution | ⚠️ implemented, awaiting runtime test |

Both paths are implemented and can run simultaneously.
The relay contact takes priority if no EEBus limit is active.

---

## Upstream / Credits

- **EEBus protocol core:** [NIBEGroup/openeebus](https://github.com/NIBEGroup/openeebus)
  — Copyright 2025 NIBE AB, Apache 2.0
- **ESP32 port and ESPHome components:** bgewehr, Apache 2.0

See [NOTICE](NOTICE) for full attribution.

---

## License

Apache License 2.0 — see [LICENSE](LICENSE).

This project is not affiliated with NIBE AB, Bosch Thermotechnik GmbH,
Shell Recharge Solutions, or Westnetz GmbH.
