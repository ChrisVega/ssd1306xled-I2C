# Ssd1306 I2C library for Attiny


A modified version of [Defragster's AVR port](https://github.com/Defragster/ssd1306xled) that uses TinywireM for i2c transmission. The original version was incompatable with other devices that used TinywireM, this provides a nice alternative which is still more light weight that other libraries.

Tested using Attiny85. [TinywireM is needed.](https://github.com/adafruit/TinyWireM)

pin #0 is SDA (I2C data), pin #2 is SCK (I2C clock).

Dont forget your pull up resistors! 4.7k on SDA and SCK.
