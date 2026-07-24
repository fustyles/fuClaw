

==================================================
CONFIRMED HARDWARE DEVICES
==================================================

Only the following device mappings are confirmed and may be directly controlled.

ESP32-S3-WROOM-1-N16R8
- GPIO_SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- ADC: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
- PWM: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- Indicator Light  : LED_BUILTIN

External Modules (Confirmed)
- Living Room Light     : GPIO 41  (analog output, range: 0-255)
- Bedroom Light     : GPIO 42  (analog output, range: 0-255)
- Light sensor         : GPIO 3  (analog input, range: 0-255)
- Bottom-hung window (SG90): GPIO 47 (servo, range: 90-180, close:90, open:180)
- Door (SG90): GPIO 48 (servo, range: 90-180, close:180, open:90)
- DHT11 Sensor: GPIO 21
- Electric Fan (Motor driver): GPIO 46, GPIO 14 (analog output, valid range: 0-100)
  - off: (0, 0);
  - full speed: (100, 0)
- Text Display (OLED Display SSD1306, I2C):
  - SDA: GPIO 1
  - SCL: GPIO 2
  - Resolution: 128x64 pixels
  - Display lines: 4
  - Supports UTF-8 text (including Traditional Chinese, English, numbers, and symbols)
  - Maximum 21 ASCII characters per line (actual capacity depends on character width and font)
  - Writable only
  - Supports text display only

Only the external hardware mappings listed above are confirmed.
Do not assume any additional hardware is connected to other GPIO pins.
Unknown hardware mappings require clarification.
GPIO values are strictly validated before execution.

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

