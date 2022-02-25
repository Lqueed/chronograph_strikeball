# chronograph_strikeball
Easy-made strikeball chronograph based on Adruino

ALL RIGHT FOR ORIGINAL IDEA BELONGS TO AlexGyver https://alexgyver.ru
https://github.com/AlexGyver/
This is sort-of a fork of his project with different components and functions, with better controls.
Differences:
- 4-button control ("menu", "reset", "+", "-")
- OLED-display
- 5 shots memory

Screens:
1. Main screen with last shot speed and energy
2. Last 5 shots speed
3. Rapitity measuring mode
4. BB's mass settings (default 0.2g, step 0.01g)

-----

Strikeball chronograph, based on Arduino Nano with I2C 128x64 OLED display (Adafruit_SSD1306)
Used for measuring BB's speed.
Powered from Arduino's USB.

Details needed:
- Arduino Nano
- I2C display 128x64px
- 4x analog buttons
- 2x L-53P3C phototransistors
- 2x L-53F3C LED diodes
- 2x 220Ohm resistors
- 2x variable resistors up to 30-50kOhm (e.g. 3296W-1-503LF)

All hardware tuning is described on the original source: https://alexgyver.ru/chron_2/

Assembly scheme:
![Chronograph](https://user-images.githubusercontent.com/26312490/155751700-0b530ed1-0a99-43a8-bd54-fac3118c9c04.png)
