# Installation

You can install the TeeRec library via the Library manager of the
Arduino IDE (Tools menu). Let it install all its dependencies. That's
it!

Make sure that you have a recent Arduino IDE (version > 2.0) and the
corresponding Teensy support (> 1.58). The next section gives some
hints for updating/installing the Arduino IDE.


## Arduino IDE and Teensyduino

Update your [Arduino IDE](https://www.arduino.cc/en/software) to
version 2 and install Teensy support.

FOr this follow the instructions for [Teensyduino](https://www.pjrc.com/teensy/td_download.html):

1. Install the latest Arduino IDE provided on
   [arduino.cc](https://www.arduino.cc/en/software).

   For Linux, download the AppImage file, make it executable, and run
   it. You may provide a link to this file in `/usr/local/bin`.

2. Run the [Arduino IDE](https://docs.arduino.cc/software/ide-v2). Go
   to File > Preferences and add
   `https://www.pjrc.com/teensy/package_teensy_index.json` to
   "Additional boards manager URLs".

3. Activate the board manager from the tool bar on the left.
   Search for Teensy and click on the "Install" button.

4. For Linux, copy the
   [udev rules](https://www.pjrc.com/teensy/00-teensy.rules)
   to `/etc/udev/rules.d/`.

5. If you are updating from Teensyduino < 1.56, make sure that you
   remove `SdFat` and `Bounce2` from your `Arduino/libraries`
   folder. They are now part of the new
   [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html).


## Installation of TeeRec from github repository

For an installation from the [TeeRec github
repository](https://github.com/janscience/TeeRec), install all the
libraries TeeRec is depending on and then clone the repository, as
described in the following. This is preferred over the installation
via the library manager if you need the latest developments or if you
need to change a few things in the library.


### Dependencies

TeeRec is based on the following libraries, which are part of the core
libraries of the Teensy boards:

- [Arduino Time Library](https://github.com/PaulStoffregen/Time)
- [ADC](https://github.com/pedvide/ADC)
- [SdFat version2](https://github.com/greiman/SdFat)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)
- A library for driving a TFT monitor. One of the optimized
  [ST7735_t3](https://github.com/PaulStoffregen/ST7735_t3) or
  [ILI9488_t3](https://github.com/PaulStoffregen/ILI9341_t3)
  libraries.

These libraries are already available and do not need to be installed
manually.

_Note_: If you upgraded from an older Arduino IDE, make sure that none
of these librariers are located in your `Arduino/libraries` folder. In
particular, `SdFat` and `Bounce2`.

The only library that needs to be installed is

- [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)

Install this library from the library manager of the Arduino IDE
(Tools > Manage libraries). Search for the library and install it.


### Download TeeRec from github

Clone the [TeeRec](https://github.com/janscience/TeeRec) repository
directly into 'Arduino/libraries':
```sh
cd Arduino/libraries
git clone https://github.com/janscience/TeeRec.git
```

For later updates, change into the `Arduino/libraries/TeeRec` directory
and pull the updates from the directoy:
```sh
cd Arduino/libraries/TeeRec
git pull origin main
```

Alternatively, download the whole repository as a zip archive (open
https://github.com/janscience/TeeRec in your browser and click on the
green "Code" button). Then unpack the zip file:
```sh
cd Arduino/libraries
unzip ~/Downloads/TeeRec-main.zip
```

If you want to edit the TeeRec files, mark the library as developmental:
```sh
cd Arduino/libraries/TeeRec
touch .development
```
otherwise the Arduino IDE complains.

Close the Arduino IDE and open it again. Then the Arduino IDE knows
about the TeeRec library and its examples.


## Upload new TeeRec version to Arduino library manager

See [Arduino Library Manager list](https://github.com/arduino/library-registry).

1. Make sure arduino-lint was successful (by pushing something onto the repository)
2. Update the version in `library.properties`.
3. Make git tag for this version: `git tag v0.9.0`
4. Push the tag to the repository: `git push origin v0.9.0`
5. On github make a new release for this tag.
6. The library indexer will detect this new version about once an hour.
7. Check in the library manager or here: https://www.arduino.cc/reference/en/libraries/teerec/


