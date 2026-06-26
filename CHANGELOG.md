# Changelog

All notable changes to this project will be documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- `eebus_lpc` ESPHome component — EEBus LPC CS actor (receives limits from
  Westnetz CLS-Steuerbox)
- `eebus_wp` ESPHome component — EEBus LPC EG actor (sends limits to Bosch
  Compress 5800i via K40RF gateway)
- ESP32 WebSocket server adapter (`port/esp32/websocket/websocket_server_esp32`)
  using `esp_http_server` with TLS — replaces libwebsockets in openeebus
- ESP32 WebSocket client adapter (`port/esp32/websocket/websocket_client_esp32`)
  using `esp_websocket_client`
- ESP-IDF component wrapper (`port/esp32/component/CMakeLists.txt`)
- PSRAM-aware heap allocator (`port/esp32/component/eebus_malloc_esp32.h`)
  for ESP32-S3
- X.509 self-signed certificate generation on first boot (stored in NVS)
- SHIP pairing flow with web UI (accept/reject buttons, SKI display)
- SKI persistence across reboots via NVS
- Heartbeat watchdog with automatic failsafe (4200 W, §14a minimum)
- SG-Ready fallback (DO1/DO2 GPIO) for Bosch Compress 5800i
- Relay-contact fallback (DI1 GPIO) for CLS-Steuerbox
- Modbus TCP wallbox control (Shell Recharge)
- ESPHome web_server v3 HEMS status UI
- Complete example YAML (`examples/hems_14a_example.yaml`)

### Dependencies
- [NIBEGroup/openeebus](https://github.com/NIBEGroup/openeebus) — EEBus
  SHIP/SPINE/LPC protocol core (git submodule, unmodified)
