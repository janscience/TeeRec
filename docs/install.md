# Installation

You can install the TeeRec library via the Library manager of the
Arduino IDE (Tools menu). Install all its dependencies. That's it!

For a manual installation from the [TeeRec github
repository](https://github.com/janscience/TeeRec), install all the
libraries TeeRec is depending on and then clone the repository, as
described in the following.


## Dependencies

TeeRec is based on the following libraries:

- [Arduino Time Library](https://github.com/PaulStoffregen/Time)
- [ADC](https://github.com/pedvide/ADC)
- [SdFat version2](https://github.com/greiman/SdFat)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)
- [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- A library for driving a TFT monitor. Either one of the optimized
  [ST7735_t3](https://github.com/PaulStoffregen/ST7735_t3) or
  [ILI9488_t3](https://github.com/PaulStoffregen/ILI9341_t3)
  libraries, or the original but slow
  [Adafruit-ST7735](https://github.com/adafruit/Adafruit-ST7735-Library)
  or [Adafruit ILI9341 Arduino
  Library](https://github.com/adafruit/Adafruit_ILI9341).

The [Arduino Time Library](https://github.com/PaulStoffregen/Time),
[ADC](https://github.com/pedvide/ADC), [SdFat
version2](https://github.com/greiman/SdFat),
[Bounce2](https://github.com/thomasfredericks/Bounce2),
[ST7735_t3](https://github.com/PaulStoffregen/ST7735_t3), and
[ILI9488_t3](https://github.com/PaulStoffregen/ILI9341_t3) libraries
are already included in
[Teensyduino](https://www.pjrc.com/teensy/teensyduino.html). When
installing Teensyduino make sure you selected them.

_Note_: you need Arduino 1.8.19 and Teensyduino 1.56 or higher! When
installing Teensyduino simply select all libraries for installation.
Remove `SdFat` and `Bounce2` from your `Arduino/libraries` folder,
they are now supplied by Teensyduino.

For installing the
[Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)
library for the TFT monitor, open in the Arduino IDE: Tools - Manage
libraries. Search for the library and install it.


## Installation

Clone the [TeeRec](https://github.com/janscience/TeeRec) repository
directly into 'Arduino/libraries':
```sh
cd Arduino/libraries
git clone https://github.com/janscience/TeeRec.git
```

Alternatively, download the whole repository as a zip archive (open
https://github.com/janscience/TeeRec in your browser and click on the
green "Code" button). Unpack the zip file:
```sh
cd Arduino/libraries
unzip ~/Downloads/TeeRec-main.zip
```

If you want to edit the TeeRec files, mark the library as developmental:
```sh
cd Arduino/libraries/TeeRec
touch .development
```

Close the Arduino IDE and open it again. Then the Arduino IDE knows
about the TeeRec library and its examples.

