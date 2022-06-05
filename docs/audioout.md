# Audio output

Via [`AudioPlayBuffer.h`](../src/AudioPlayBuffer.h) TeeRec allows to
feed data from a [`DataWorker.h`](../src/DataWorker.h) into the
[Teensy Audio library](https://github.com/PaulStoffregen/Audio).

Below you find some comments on available hardware for audio output.


## Adafruit MAX98357 I2S Class-D Mono Amp

The [Adafruit MAX98357 I2S Class-D Mono
Amp](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp)
takes digital audio data and can drive a little speaker. Also
available in stereo. Can be driven with the `AudioOutputI2S` class from the
[Teensy Audio library](https://github.com/PaulStoffregen/Audio).

![Adafruit MAX98357
 pinout](https://cdn-learn.adafruit.com/assets/assets/000/032/613/medium800/adafruit_products_pinouts.jpg?1464025812)


## Teensy audio adaptor board

The [Audio Adaptor Boards for Teensy 3.x and Teensy
4.x](https://www.pjrc.com/store/teensy3_audio.html) is soldered as a
shield directly on the Teensy. It provides a mini stereo jack for
pluggin in headphones and line out. For connecting a speaker you need
an aditional amplifier.

![audio adaptor](https://www.pjrc.com/store/teensy3_audio.jpg)


## Teensy prop shield

The [Prop Shield With Motion
Sensors](https://www.pjrc.com/store/prop_shield.html) provides an audio
amplifier. It needs 5V power and an analog signal.

![prop shield](https://www.pjrc.com/store/propshield_wit_front.jpg)


## Stereo Enclosed Speaker Set

The [Stereo Enclosed Speaker Set - 3W 4
Ohm](https://www.adafruit.com/product/1669) work nicely on the
amplifiers commented above.

![enclodes speakers](https://cdn-shop.adafruit.com/970x728/1669-06.jpg)

