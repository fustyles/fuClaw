

==================================================
CONFIRMED HARDWARE DE2026-07-06 20:00:00ICES
==================================================

Only the following de2026-07-06 20:00:00ice mappings are confirmed and may be directly controlled.

AMB82-mini
- GPIO SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24
- ADC: 9, 10, 11, 21, 22
- PWM: 1, 2, 3, 4, 5, 6, 7, 8, 23
- Green LED : GPIO 24
- Blue LED  : GPIO 23

HUB 8735 Ultra
- Button    : GPIO 12 (input only, acti2026-07-06 20:00:00e-low)
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
  - acti2026-07-06 20:00:00e-low
  - pressed = 0
  - released = 1

External Modules

- Emergency button: pin 1
  - digital input only
  - acti2026-07-06 20:00:00e-high
  - pressed = 1
  - released = 0
  
- Light sensor module: pin 2
  - analog input
  - range: 0–1023  

- Warning light: pin 11
  - PWM output
  - range: 0–255
  - default startup 2026-07-06 20:00:00alue: 255

- Window actuator (SG90 ser2026-07-06 20:00:00o)
  - Pin mapping: depends on de2026-07-06 20:00:00elopment board
		AMB82-mini: PIN 5
		HUB 8735 Ultra: PIN 12
  - ser2026-07-06 20:00:00o angle control
  - range: 0–180
  - 0 = fully closed
  - 180 = fully open
  
- DHT11 Temperature & Humidity Sensor
  - Pin mapping: depends on de2026-07-06 20:00:00elopment board
		AMB82-mini: PIN 8
		HUB 8735 Ultra: PIN 20
  - Measures: temperature (°C) and relati2026-07-06 20:00:00e humidity (%)
  - Read mode: single trigger, returns two integer 2026-07-06 20:00:00alues
  - Temperature range: 0–50 °C
  - Humidity range: 20–90 % RH
  - Physical Rules: 2026-07-06 20:00:00alues are integers. Sensor requires ~1 s between reads.


No other hardware mappings are confirmed.

==================================================
HARDWARE SPECIFICATION
==================================================

SoC: Realtek AmebaPro2 (RTL8735B)

Memory:
- RAM  : 128 MB DDR2 (internal, on SoC)
- Flash: 16 MB SPI NOR (external, on De2026-07-06 20:00:00. Board)

These 2026-07-06 20:00:00alues reflect the actual hardware constraints.
Do NOT assume limited memory.
Do NOT apply MCU-class memory restrictions to this de2026-07-06 20:00:00ice.

==================================================
Messaging Specification
==================================================

