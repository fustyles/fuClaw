

==================================================
CONFIRMED HARDWARE DEVICES
==================================================

Only the following device mappings are confirmed and may be directly controlled.

AMB82-mini
- GPIO SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24
- ADC: 9, 10, 11, 21, 22
- PWM: 1, 2, 3, 4, 5, 6, 7, 8, 23
- Green LED : GPIO 24
- Blue LED  : GPIO 23

HUB 8735 Ultra
- Button    : GPIO 12 (input only, active-low)
- GPIO SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24,25,26
- ADC: 0, 1, 2, 9, 10
- PWM: 11, 12, 13, 19, 20, 21, 22, 23, 24
- Green LED : GPIO 25
- Blue LED  : GPIO 26
- Fill LED  : GPIO 13
  - analog output range: 0–255
  - recommended safe startup brightness: 5
- Function button: pin 12
  - digital input only
  - active-low
  - pressed = 0
  - released = 1

External Modules

- Emergency button: pin 1
  - digital input only
  - active-high
  - pressed = 1
  - released = 0
  
- Light sensor module: pin 2
  - analog input
  - range: 0–1023  

- Warning light: pin 11
  - PWM output
  - range: 0–255
  - default startup value: 255

- Window actuator (SG90 servo)
  - Pin mapping: depends on development board
		AMB82-mini: PIN 5
		HUB 8735 Ultra: PIN 12
  - servo angle control
  - range: 0–180
  - 0 = fully closed
  - 180 = fully open
  
- DHT11 Temperature & Humidity Sensor
  - Pin mapping: depends on development board
		AMB82-mini: PIN 8
		HUB 8735 Ultra: PIN 20
  - Measures: temperature (°C) and relative humidity (%)
  - Read mode: single trigger, returns two integer values
  - Temperature range: 0–50 °C
  - Humidity range: 20–90 % RH
  - Physical Rules: Values are integers. Sensor requires ~1 s between reads.


No other hardware mappings are confirmed.

==================================================
HARDWARE SPECIFICATION
==================================================

SoC: Realtek AmebaPro2 (RTL8735B)

Memory:
- RAM  : 128 MB DDR2 (internal, on SoC)
- Flash: 16 MB SPI NOR (external, on Dev. Board)

These values reflect the actual hardware constraints.
Do NOT assume limited memory.
Do NOT apply MCU-class memory restrictions to this device.

==================================================
Messaging Specification
==================================================

