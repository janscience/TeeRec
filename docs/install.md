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

For this follow the instructions for
[Teensyduino](https://www.pjrc.com/teensy/td_download.html):

1. Install the latest Arduino IDE provided on
   [arduino.cc](https://www.arduino.cc/en/software).

   For Linux, download the AppImage file. See below for details.
   
2. Run the [Arduino IDE](https://docs.arduino.cc/software/ide-v2). Go
   to File > Preferences and add
   `https://www.pjrc.com/teensy/package_teensy_index.json` to
   "Additional boards manager URLs".

3. Activate the board manager from the tool bar on the left.
   Search for Teensy and click on the "Install" button.
   
4. If you are updating from Teensyduino < 1.56, make sure that you
   remove `SdFat` and `Bounce2` from your `Arduino/libraries`
   folder. They are now part of the new
   [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html).

That's all!


### Linux installation hints

1. After downloading the AppImage, make it executable and provide a
   link to this file in `/usr/local/bin`.
   ```sh
   cd Downloads
   chmod a+x arduino-ide_2.3.4_Linux_64bit.AppImage
   sudo ln -s arduino-ide_2.3.4_Linux_64bit.AppImage /usr/local/bin/arduino
   ```
   Now you can run it by executing `arduino` anywhere.

2. If the Arduino IDE complains about sandboxing, then you need to
   tweak apparmor settings (this happens on Ubuntu 24, for
   example). Create a file
   `home.<username>.Downloads.arduino-ide_2.3.4_Linux_64bit.AppImage`
   in `/etc/apparmor.d/`, e.g. via
   
   ```sh
   sudo vim /etc/apparmor.d/home.<username>.Downloads.arduino-ide_2.3.4_Linux_64bit.AppImage
   ```
   Copy the following content into this file:
   ```txt
   abi <abi/4.0>,
   include <tunables/global>
   profile arduino /home/<username>/Downloads/arduino-ide_2.3.4_Linux_64bit.AppImage flags=(unconfined) {
     userns,
     include if exists <local/arduino>
   }
   ```
   Adapt the folders, arduino files name and `<username>` as needed.

   Save the file and run
   ```sh
   sudo service apparmor reload
   ```

   See [ask Ubuntu](https://askubuntu.com/questions/1515105/sandbox-problems-with-arduino-ides-with-24-04) for details and alternatives.
   
4. Copy the [udev rules](https://www.pjrc.com/teensy/00-teensy.rules)
   to `/etc/udev/rules.d/`:
   ```sh
   cd ~/.arduino15/packages/teensy/tools/teensy-tools/1.59.0
   sudo cp 00-teensy.rules /etc/udev/rules.d/
   sudo udevadm trigger
   ```
   These rules are needed to make the Teensy USB device readable and
   writable for all users.


## Installation of TeeRec from github repository

If you want to use the latest development versions of TeeRec, or
change a few things in the code, then do not install TeeRec via the
library manager of the Arduino IDE but download TeeRec directly from
the [TeeRec github repository](https://github.com/janscience/TeeRec) as described in the following.

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
of these libraries are located in your `Arduino/libraries` folder. In
particular, `SdFat` and `Bounce2`.

The only library that needs to be installed is

- [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)

Install this library from the library manager of the Arduino IDE
(Tools > Manage libraries). Search for the library and install it.


### Download TeeRec from github

Clone the [TeeRec](https://github.com/janscience/TeeRec) repository
directly into the 'Arduino/libraries' directory:
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


## Publish new TeeRec version on Arduino library registry

This is a note to myself...

See [Arduino Library Manager list](https://github.com/arduino/library-registry).

1. Make sure arduino-lint was successful (by pushing something onto the repository)
2. Update the version in `library.properties`.
3. Make git tag for this version: `git tag v0.9.0`
4. Push the tag to the repository: `git push origin v0.9.0`
5. On github make a new release for this tag.
6. The library indexer will detect this new version about once an hour.
7. Check in the library manager or here: https://www.arduino.cc/reference/en/libraries/teerec/


