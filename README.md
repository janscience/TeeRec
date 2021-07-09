# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.5/3.6 microcontrollers.

Based on Pedro Villanueva's [ADC](https://github.com/pedvide/ADC)
library, Paul Stoffregen's
[DMAChannel](https://github.com/PaulStoffregen/cores/tree/master/teensy3),
Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library, and
on Stefan Mucha's first
[EOD logger sketch](https://github.com/muchaste/EOD-Logger/tree/master/eodlogger_v1)
with contributions by
[Lydia Federman](https://github.com/LydiaFe/EOD-Logger)
(simultaneous recordings from both ADCs) and
[Sebastian Volkmer](https://github.com/SebastianVol/EOD-Logger/blob/master/eodlogger_2channel_barebone/eodlogger_8channel.ino)
(multiplexing).


## Features

- Continuous DMA-based data acquisition into a single multiplexed circular buffer.
- Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
- Highspeed timed acquisition up to 500kHz.
- Conversion of data to signed 16bit for direct storage into WAV files.
- Detailed metadta in wave file header: sampling rate, number of
  channels and pin IDs, bit resolution, date and time, Teensy board
  version and its unique MAC address..
- Storing recorded data as wave files on SD cards.
- Display recorded data on a monitor.
- Generate test signals.
- React to push buttons.
- Feedback by blinking the LED.
- Configurable via config file on SD card.


## Libraries

- DataBuffer: A single cyclic, multiplexed buffer holding acquired data.
- ContinousADC: Sample from multiple pins into a DataBuffer.
- SDWrite: Write data on SD card.
- WaveHeader: Setting up wave file header with metadata.
- Display: Display data on a monitor.
- RTClock: Time and date strings from the real time clock.
- Blink: Blinking LEDs.
- PushButtons: Manage and query push buttons with callback functions.
- TestSignals: Generate test signals on pulse-width modulation pins.
- TeensyBoard: Find out which Teensy board we are running on.
- Configurable: Base class for all configurable classes.
- Settings: Common configurable settings (file name, path, etc.)
- Configurator: Configure configurable class instances from a configuration file.


## Examples

- scope: Show acquired data on a display.
- logger: Continuously store data on SD card.
- recorder: Show acquired data on a display and store data on SD card upon user request.
- averaging: Test various averaging settings for acquisition.
- writeconfig: Write a defaul configuration file on SD card.
- removefiles: Remove all files from a directory on SD card.
- teensyboard: Print Teensy board version, serial number and MAC adress.
- blink: Demonstrate usage of Blink class.
- pushbuttons: Demonstrate usage of PushButtons class.


## Dependencies

TeeRec is based on the following libraries:

- [Arduino Time Library](https://github.com/PaulStoffregen/Time)
- [ADC](https://github.com/pedvide/ADC)
- [SdFat version2](https://github.com/greiman/SdFat)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)

The Display library currently uses:

- [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library)
- [Adafruit ILI9341 Arduino Library](https://github.com/adafruit/Adafruit_ILI9341)


The [Arduino Time Library](https://github.com/PaulStoffregen/Time) and
[ADC](https://github.com/pedvide/ADC) libraries are already included
in [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html). When
installing Teensyduino make sure you selected them.

For installing [SdFat version2](https://github.com/greiman/SdFat),
[Bounce2](https://github.com/thomasfredericks/Bounce2), and the
libraries for the TFT monitor, open in the Arduino IDE: Tools - Manage
libraries. Search for "SdFat", "Bounce2" and the TFT libraries and
install them.


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


## Links

- [EOD-Logger](https://github.com/muchaste/EOD-Logger)
- [Audio](https://github.com/PaulStoffregen/Audio) library
- microSoundRecorder - Environmental Sound Recorder for Teensy 3.6:
  [github](https://github.com/WMXZ-EU/microSoundRecorder)
  [wiki](https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup)
  [forum](https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386)