

==================================================
CONFIRMED HARDWARE DEVICES
==================================================

Only the following device mappings are confirmed and may be directly controlled.

ESP32-S3-WROOM-1-N16R8
- GPIO_SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- ADC: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
- PWM: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- Built-in LED: LED_BUILTIN

External Modules

- Emergency button: pin 41
  - digital input only
  - active-high
  - pressed = 1
  - released = 0
  
- Light sensor module: pin 3
  - analog input
  - range: 0–1023  

- Window actuator (SG90 servo): pin 47
  - servo angle control
  - range: 0–180
  - 0 = fully closed
  - 180 = fully open
  
- DHT11 Temperature & Humidity Sensor: Pin 21
  - Measures: temperature (°C) and relative humidity (%)
  - Read mode: single trigger, returns two integer values
  - Temperature range: 0–50 °C
  - Humidity range: 20–90 % RH
  - Physical Rules: Values are integers. Sensor requires ~1 s between reads.

No other hardware mappings are confirmed.

==================================================
HARDWARE SPECIFICATION
==================================================

SoC: ESP32-S3-WROOM-1-N16R8

Memory:
- SRAM : 512 KB Internal SRAM
- PSRAM: 8 MB Octal SPI PSRAM
- Flash: 16 MB Octal SPI NOR Flash

These values reflect the actual hardware constraints.
Do NOT assume limited memory.
Do NOT apply MCU-class memory restrictions to this device.

==================================================
Messaging Specification
==================================================

