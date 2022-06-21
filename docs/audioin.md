# Audio input

Here you find some comments on available hardware for audio input via
microphone.


## SparkFun Analog MEMS Microphone Breakout

The [SparkFun Analog MEMS Microphone Breakout
(BOB-18011)](https://www.sparkfun.com/products/18011) uses the
InvenSense ICS-40180 analog microphone that is amplified with an
OpAmp.

![SparkFun Analog MEMS Microphone Breakout pinout](https://cdn.sparkfun.com//assets/parts/1/7/2/2/1/18011-SparkFun_Analog_MEMS_Microphone_Breakout_-_ICS-40180-02.jpg)

| MEMS Mic | Teensy                      | Function    |
| -------- | --------------------------- | ----------- |
| AUD      | any analog input, e.g. A0   | Audio data  |
| GND      | GND                         | Ground      |
| VCC      | 3.3V                        | Input Power |

The analog output data is centered around half of VCC. That is 1.65V
for VCC=3.3V. Unfortunately the `AudioInputAnalog` from the Audio
library uses 1.2V as a reference and not 3.3V. You can modify this on
line 55 in `input_adc.cpp`: change `analogReference(INTERNAL);` to
`analogReference(EXTERNAL)`.

Or use the `ContinousADC` class from `TeeRec`.

This microphone has a nice sensitivity!


## Adafruit Silicon MEMS Microphone Breakout - SPW2430

The [Adafruit Silicon MEMS Microphone
Breakout](https://www.adafruit.com/product/2716) is based on the
SPW2430 MEMS microphone that transduces sound into an analog
voltage. This signal needs to be connected to an analog input pin.
The signal osscilates arounf 0.67V, so you should configure the analog
input to the 1.2V reference voltage.

![Adafruit Silicon MEMS pinout](https://cdn-shop.adafruit.com/970x728/2716-06.jpg)

| MEMS Mic | Teensy                    | Function                |
| -------- | ------------------------- | ----------------------- |
| Vin      | 3.3V                      | Input Power             |
| 3V       | -                         | 3V                      |
| GND      | GND                       | Ground                  |
| AC       | -                         | AC coupled audio signal |
| DC       | any analog input, e.g. A2 | Audio signal with 0.67V offset |

Recording from this microphone using AudioInputAnalog from the Audio
library is noisy and not very sensitive.


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

The sensitivity of the microphone is somewhat low. That is, if you
normally speak into it at a short distance, the output is far below
the full 16bit.

