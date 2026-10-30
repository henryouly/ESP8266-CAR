# AGENTS.md

Notes for working on this repo (ESP8266 Wi-Fi car firmware + Android client).

## Firmware toolchain

Installed and verified on this machine (macOS, Apple Silicon):

```sh
brew install arduino-cli
arduino-cli config init --overwrite
arduino-cli config add board_manager.additional_urls \
  https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
arduino-cli core install esp8266:esp8266
```

* Board FQBN: `esp8266:esp8266:nodemcuv2` (NodeMCU 1.0 / ESP-12E).
* Serial baud: `115200`.

### Apple Silicon gotcha

The `esp8266` core ships an **x86_64** Python tool. On arm64 Macs builds fail with
`bad CPU type in executable` until Rosetta 2 is installed:

```sh
softwareupdate --install-rosetta --agree-to-license
```

### Sketch folder naming

`arduino-cli` requires the sketch folder name to match the `.ino` filename, but the
sketch lives at the repo root (`ESP8266-CAR/esp8266_car.ino`). Do **not** restructure
the repo; compile via a temp folder containing a symlink:

```sh
TMP=/var/folders/qk/vx2bnbgd53n7t302hr4y8l180000gn/T/opencode/esp8266_car
mkdir -p "$TMP"
ln -sf "$PWD/esp8266_car.ino" "$TMP/esp8266_car.ino"
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 "$TMP"
```

## Flashing

The board uses a **Silicon Labs CP2102** USB-UART bridge; macOS enumerates it
natively (no extra driver on recent macOS). Port appears as `/dev/cu.usbserial-0001`
(number may vary).

```sh
arduino-cli board list
arduino-cli upload -p /dev/cu.usbserial-0001 --fqbn esp8266:esp8266:nodemcuv2 "$TMP"
arduino-cli monitor -p /dev/cu.usbserial-0001 -c baudrate=115200
```

Troubleshooting:

* If `ls /dev/cu.*` shows no USB port, check `ioreg -p IOUSB -w0 -l` — no
  `CP2102` entry means a charge-only cable, hub, or bad adapter, not a driver issue.
* If upload fails, hold the FLASH/BOOT button while plugging in to force flash mode.

## After flashing

The board starts a Wi-Fi AP (`NodeMCU_Car` / `nodemcucar`, IP `192.168.4.1`) and an
HTTP server on port 80. Endpoints are documented in `README.md`.

## Known firmware bugs

* `pwm2` route is registered as `"pwm2"` without a leading slash
  (`esp8266_car.ino:49`), so it is unreachable — should be `"/pwm2"`.
* `D0` is GPIO16 (no hardware PWM); `PWM_PIN` uses `D0`/`D2` (`esp8266_car.ino:18`),
  so left-motor speed control via `pwm1` does not work. Prefer `D1`/`D2`.
* AP credentials, IP, and endpoints are hardcoded with no authentication.

## Android app

Legacy toolchain (AGP 2.2.0, Gradle 2.14.1, `compileSdk` 25, `jcenter()`); it will
not build on current Android Studio without migration to AGP 8.x/AndroidX.
