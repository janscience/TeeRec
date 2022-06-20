# Audio input

Below you find some comments on available hardware for audio input.

## Adafruit I2S MEMS Microphone Breakout

The [Adafruit I2S MEMS Microphone
Breakout](https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout)
is a SPH0645 MEMS microphone that transmits data via I2S. Can be read
with the the `AudioInputI2S` class from the [Teensy Audio
library](https://github.com/PaulStoffregen/Audio).

![Adafruit I2S MEMS
 pinout](https://cdn-learn.adafruit.com/guides/cropped_images/000/001/592/medium640/pintou.jpg?1520544902)

Connections:

| I2S Mic  | Teensy                      | Function               |
| -------- | --------------------------- | ---------------------- |
| SEL      | (GND) | Channel select pin (low (default) transmits data on left channel) |
| LRCL     | 23                          | Audio Left/Right Clock |
| DOUT     | 13                          | Audio Data Out         |
| BCLK     | 9                           | Audio Bit Clock        |
| 3V       | 3.3V                        | Input Power            |
| GND      | GND                         | Ground                 |
