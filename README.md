# My project running a car with a ESP8266 chipset

**Hardware:**
* L298N
* NodeMCU 1.0
* Four wheel car with 4 motors.

**Wiring:**
* Connect L298N +12V to NodeMCU Vin PIN
* Connect L298N GND to NodeMCU GND PIN
* Leave out L298N +5V open.
* Connect L298N IN1, IN2, IN3, IN4 to NodeMCU D3, D4, D5, D6 PIN
* Combile motors into two groups - connect the left group to OUT2; right group to OUT3.

**Control:**
http://192.168.4.1/{command}

{command} can be "forward", "backward", "left", "right", "stop"
