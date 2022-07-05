![arduino-lint](https://github.com/janscience/TeeRec/actions/workflows/arduino-lint.yml/badge.svg)

# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.2/3.5/3.6 microcontrollers.

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

- Flexible producer/consumer data model based on a single multiplexed circular buffer.
- Continuous DMA-based data acquisition.
- Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
- Highspeed timed acquisition up to 500kHz.
- Conversion of data to signed 16bit for direct storage into wave files.
- Continuous storing recorded data as wave files on SD cards.
- Detailed metadata in wave file header: sampling rate, number of
  channels and pin IDs, bit resolution, date and time, Teensy board
  version, and its unique MAC address.
- Audio monitor.
- Display recorded data on a monitor.
- Generate test signals.
- React to push buttons.
- Feedback by blinking the LED.
- Configurable via config file on SD card.

For slow acquisition of environmental sensor readings see [ESensors
library](https://github.com/janscience/ESensors).


## Documentation

- [Installation instructions](docs/install.md)
- [Audio output](docs/audioout.md): hardware for audio output.
- [Audio input](docs/audioin.md): hardware for audio input (microphones).


## Libraries

The features provided by TeeRec are provided by many C++ classes,
defined in the following libraries. You can include them all at once
via the `TeeRec.h` header.

### Data acquisition

- [DataBuffer](src/DataBuffer.h): A single cyclic, multiplexed buffer holding acquired data.
- [DataWorker](src/DataWorker.h): Producer/consumer working on a DataBuffer.
- [ContinuousADC](src/ContinuousADC.h): Sample from multiple pins into a DataBuffer.

### Storage on SD card

- [SDWrite](src/SDWrite.h): Write data from a DataWorker to SD card.
- [WaveHeader](src/WaveHeader.h): Setting up wave file header with metadata.

### Configuration

- [Configurable](src/Configurable.h): Base class for all configurable classes.
- [Settings](src/Settings.h): Common configurable settings (file name, path, etc.)
- [Configurator](src/Configuration.h): Configure configurable class instances from a configuration file.

### Audio monitor

- [AudioPlayBuffer](src/AudioPlayBuffer.h): Make the DataBuffer available as an input for the Audio library.
- [AudioMonitor](src/AudioMonitor.h): Play recorded data with optional feedback signals on speaker.

### Online analysis

- [AnalysisChain](src/AnalysisChain.h): Coordinate analysis of data snippets via Analyzer.
- [Analyzer](src/Analyzer.h): Base class for analyzer called by AnalysisChain.

### Utilities

- [Display](src/Display.h): Display data on a TFT monitor.
- [AllDisplays](src/AllDisplays.h): Include selected TFT library for the examples.
- [RTClock](src/RTClock.h): Time and date strings from the real time clock.
- [Blink](src/Blink.h): Blinking LEDs.
- [PushButtons](src/PushButtons.h): Manage and query push buttons with callback functions.
- [TestSignals](src/TestSignals.h): Generate test signals on pulse-width modulation and DAC pins.
- [TeensyBoard](src/TeensyBoard.h): Find out which Teensy board we are running on.


## Examples

In [examples/](examples) you find sketches demonstrating the use of
the TeeRec libraries.

### Data acquisition

These examples could be used as the basis for you data acquisition application.

- [scope](examples/scope): Show acquired data on a display.
- [logger](examples/logger): Continuously store data on SD card.
- [recorder](examples/recorder): Show acquired data on a display and store data on SD card upon user request.
- [audioscope](examples/audioscope): Play acquired data on speaker and display them on a monitor.
- [audiorecorder](examples/audiorecorder): Play acquired data on speaker and store data on SD card upon user request.

### Testing data acquisition

Useful sketches for checking out the performance of the data acquisition.

- [zero](examples/zero): Report mean and standard deviation of recorded signal.
- [maxrate](examples/maxrate): Test for maximum possible sampling rate.
- [averaging](examples/averaging): Test various averaging settings for acquisition.

### Utilities

Some useful utilities.

- [sinegen](examples/sinegen): Sine-wave generator.
- [teensyboard](examples/teensyboard): Print Teensy board version, serial number and MAC adress.
- [writeconfig](examples/writeconfig): Write a default configuration file on SD card.
- [removefiles](examples/removefiles): Remove all files from a directory on SD card.

### Demos

Demonstrate the usage of some of the provided libraries.

- [temperature](examples/temperature): Read temperature from 1-wire device.
- [blink](examples/blink): Demonstrate usage of Blink class.
- [pushbuttons](examples/pushbuttons): Demonstrate usage of PushButtons class.


## Utilities

In [utils/](utils) you find some useful python scripts.

- [viewwave](utils/viewwave): display the traces in a wave file.
- [continuity](utils/continuity): check whether pulse signals recorded into wave file have consistent periods over many wave files.
- [cycles](utils/cycles): plot failures in pulse traces? - needs update.
- [noise](utils/noise): plot and analyse baseline noise levels from wave files.
- [spectra](utils/spectra): power spectra of traces in wave files.


## TODO

- Add more sensors.
- Extend interface to allow for checking whether a sensor reading is available. 


## Applications of the TeeRec libraries

TeeRec is used in:

- [EOD-Logger](https://github.com/muchaste/EOD-Logger): A 2-electrode
  logger for recording electric-organ discharges of electric fish.

- [TeeGrid](https://github.com/janscience/TeeGrid): Electrode arrays
  based on 8-16channel recording devices for recording electric fish
  behavior in natural habitats (see [Henninger et
  al. 2018](https://doi.org/10.1523/JNEUROSCI.0350-18.2018) and
  [Henninger et al. 2020](https://doi.org/10.1242/jeb.206342)).

- [FishFinder](https://github.com/janscience/FishFinder): Smart
  fishfinders for better EOD recordings of electric fish in the field.


## Links

- [Teensy Audio library](https://github.com/PaulStoffregen/Audio)
- [microSoundRecorder](https://github.com/WMXZ-EU/microSoundRecorder) - Environmental Sound Recorder for Teensy 3.6:
  [[github]](https://github.com/WMXZ-EU/microSoundRecorder)
  [[wiki]](https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup)
  [[forum]](https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386)