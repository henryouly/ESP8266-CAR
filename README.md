# ESP8266 Car

A Wi-Fi controlled four-wheel car built on a NodeMCU 1.0 (ESP8266) and an L298N
motor driver. The car exposes its own Wi-Fi access point and a small HTTP
server; an Android app (tilt control) and any browser/curl client can drive it.

```
Phone (Android app)  --HTTP GET-->  192.168.4.1  (NodeMCU AP "NodeMCU_Car")
                                        │
                                   esp8266_car.ino
                                        │ GPIO
                                     L298N driver ── 4 motors
```

## Repository layout

| Path | Description |
| --- | --- |
| `esp8266_car.ino` | ESP8266 firmware: Wi-Fi AP + HTTP control server |
| `android/` | Android (Java) tilt-control app |

## Hardware

* NodeMCU 1.0 (ESP8266)
* L298N dual H-bridge motor driver
* Four-wheel car chassis with 4 DC motors
* 12 V battery pack

## Wiring

* Connect L298N `+12V` to NodeMCU `Vin` pin
* Connect L298N `GND` to NodeMCU `GND` pin
* Leave L298N `+5V` open
* Connect L298N `IN1`, `IN2`, `IN3`, `IN4` to NodeMCU `D3`, `D4`, `D5`, `D6`
* Combine the motors into two groups: left group on `OUT2`, right group on `OUT3`
* L298N `ENA`/`ENB` go to NodeMCU `D0`/`D2` for PWM speed control (see
  [Known issues](#known-issues) about `D0`)

> Note: feeding 12 V into NodeMCU `Vin` works but stresses the onboard regulator.
> A separate buck converter to 5 V is recommended for the NodeMCU.

## Firmware

The sketch:

1. Configures the direction pins (`D3`–`D6`) and PWM pins (`D0`, `D2`).
2. Starts a soft access point: SSID `NodeMCU_Car`, password `nodemcucar`
   (IP `192.168.4.1`).
3. Serves the control HTTP API on port 80.

### HTTP API

Connect to the `NodeMCU_Car` Wi-Fi network, then:

| Request | Action |
| --- | --- |
| `GET http://192.168.4.1/` | HTML page with control buttons |
| `GET http://192.168.4.1/forward` | Drive forward |
| `GET http://192.168.4.1/backward` | Drive backward |
| `GET http://192.168.4.1/left` | Turn left |
| `GET http://192.168.4.1/right` | Turn right |
| `GET http://192.168.4.1/stop` | Stop |
| `GET http://192.168.4.1/pwm1?pwm=N` | Set left motor PWM (0–1023) |
| `GET http://192.168.4.1/pwm2?pwm=N` | Set right motor PWM (0–1023), currently broken |

Direction mapping lives in `PIN_DEF` (`esp8266_car.ino:10`).

### Building and flashing

The `.ino` file sits at the repository root, while the Arduino IDE requires the
sketch folder to be named `esp8266_car`. Use one of:

**PlatformIO (recommended)** — add `platformio.ini`:

```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
```

Then build/upload with `pio run -t upload` and watch the serial log with
`pio device monitor`.

**Arduino IDE** — install the ESP8266 board package, then either rename/copy the
folder to `esp8266_car/` or let the IDE relocate the sketch. Select board
"NodeMCU 1.0 (ESP-12E Module)".

Serial output is `115200` baud and prints the AP IP on boot.

## Android app

`android/app/src/main/java/com/github/henryouly/car/MainActivity.java` reads the
device orientation sensor and sends commands to the car:

* `values[2]` > 75 → `backward`; < 20 → `stop`
* `values[1]` > 20 → `left`; < -20 → `right`
* otherwise → `forward`

Repeated commands are de-duplicated. The activity is locked to landscape.

The project targets a 2017 toolchain (AGP 2.2.0, Gradle 2.14.1, `compileSdk`
25, support library 25.3.1, `jcenter()`), so it does not build on current
Android Studio without migration. To build it today you would need to upgrade
to AGP 8.x/JDK 17, migrate to AndroidX, replace `jcenter()`, remove the
deprecated `compile` configuration names, and allow cleartext HTTP traffic to
`192.168.4.1`.

## Known issues

* `pwm2` route is registered without a leading slash (`esp8266_car.ino:49`), so
  it is unreachable.
* `D0` is GPIO16, which has no hardware PWM on the ESP8266 — use `D1`/`D2` for
  PWM if speed control is required.
* The AP password and server IP are hardcoded, and the HTTP endpoints have no
  authentication; use only on a private bench/network.
* `TYPE_ORIENTATION` is deprecated, and the tilt mapping assumes a specific
  phone orientation; verify on the real device.
