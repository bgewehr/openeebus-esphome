# Contributing to openeebus-esphome

Thank you for your interest in contributing! This project welcomes issues,
bug reports, and pull requests.

## What this project covers

This repo provides the **ESP32 port** and **ESPHome external components** only.
The EEBus SHIP/SPINE protocol implementation lives upstream in
[NIBEGroup/openeebus](https://github.com/NIBEGroup/openeebus) — protocol bugs
should be reported there.

Contributions welcome here:
- ESP32 WebSocket adapter (`port/esp32/`)
- ESPHome component logic (`components/eebus_lpc/`, `components/eebus_wp/`)
- Example YAML configurations (`examples/`)
- Documentation and translations
- Test reports from real hardware (CLS-Steuerbox, K40RF, other EEBus devices)

## Getting started

```bash
git clone --recurse-submodules https://github.com/bgewehr/openeebus-esphome.git
cd openeebus-esphome
```

## Pull requests

1. Fork the repo and create a branch from `main`
2. Keep changes focused — one feature or fix per PR
3. Update `examples/` if you add a new configuration option
4. Describe your hardware setup and test results in the PR description

## Reporting bugs

Use the **Bug report** issue template. Please include:
- ESPHome version
- ESP32 board model
- EEBus device type (CLS-Steuerbox model, K40RF firmware version)
- Relevant log output (`logger: level: DEBUG`)

## Code style

- C/C++: follow the existing style (K&R braces, `snake_case`)
- Python (`__init__.py`): follow ESPHome conventions
- YAML examples: well-commented, no hardcoded secrets

## License

By contributing you agree that your contributions will be licensed under the
Apache License 2.0, the same license as this project.
