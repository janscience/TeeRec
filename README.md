![arduino-lint](https://github.com/janscience/TeeRec/actions/workflows/arduino-lint.yml/badge.svg)

# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.2/3.5/3.6/4.0/4.1 microcontrollers.

Recording from the Teensy's internal ADCs
([InputADC](src/InputADC.h)) is based on Pedro Villanueva's
[ADC](https://github.com/pedvide/ADC) library, Paul Stoffregen's
[DMAChannel](https://github.com/PaulStoffregen/cores/tree/master/teensy3),
Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library, and
on Stefan Mucha's first [EOD logger
sketch](https://github.com/muchaste/EOD-Logger/tree/master/eodlogger_v1)
with contributions by [Lydia
Federman](https://github.com/LydiaFe/EOD-Logger) (simultaneous
recordings from both ADCs) and [Sebastian
Volkmer](https://github.com/SebastianVol/EOD-Logger/blob/master/eodlogger_2channel_barebone/eodlogger_8channel.ino)
(multiplexing).

Recording a TDM data stream ([InputTDM](src/InputTDM.h)) is based on
the [Teensy Audio library](https://github.com/PaulStoffregen/Audio)
and the setI2SFreq() function introduced by Frank B on the [Teensy
forum](https://forum.pjrc.com/threads/38753-Discussion-about-a-simple-way-to-change-the-sample-rate/page4).

## Features

- Flexible producer/consumer data model based on a single multiplexed circular buffer.
- Multiplexed acquisition from multiple channels, from one or both
  ADCs or TDM streams.
- Conversion of data to signed 16bit for direct storage into wave files.
- Continuous storing of recorded data as wave files on SD cards.
- Detailed metadata in wave file header: sampling rate, number of
  channels and pin IDs, bit resolution, gain, date and time, Teensy board
  version, and its unique MAC address.
- Filenames based on date and/or time of the real-time-clock or incremental.
- Analysis chain on top of data buffer.
- Audio monitor.
- Display recorded data on a monitor.
- Generate test signals.
- React to push buttons.
- Read out a device identifier.
- Feedback by blinking the LED.

For slow acquisition of environmental sensor readings see [ESensors
library](https://github.com/janscience/ESensors).


## Documentation

- [Installation instructions](docs/install.md)
- [Performance of Teensy ADC](docs/inputadc.md)
- [Audio output](docs/audioout.md): hardware for audio output.
- [Audio input](docs/audioin.md): hardware for audio input (microphones).
- [Data rates](docs/datarates.md): data rates for a range of sampling rates and channels.


## Libraries

The features provided by TeeRec are provided by many C++ classes,
defined in the following libraries. You can include them all at once
via the `TeeRec.h` header.

### Data acquisition

- [DataBuffer](src/DataBuffer.h): A single cyclic, multiplexed buffer holding acquired data.
- [DataWorker](src/DataWorker.h): Producer/consumer working on a DataBuffer.
- [Input](src/Input.h): Base class for all input streams.
- [InputADC](src/InputADC.h): Sample from multiple analog pins into a DataBuffer. Also see [Performance of Teensy ADC](docs/inputadc.md).
- [InputTDM](src/InputTDM.h): Streaming TDM data into a single cyclic buffer.
- [Device](src/Device.h): General device infos.
- [ControlPCM186x](src/ControlPCM1865.h): Control a TI PCM186x chip.

### Storage on SD card

- [SDCard](src/SDCard.h): Oparate on SD cards.
- [SDWriter](src/SDWriter.h): Write data from a DataWorker to SD card.
- [WaveHeader](src/WaveHeader.h): Setting up wave file header with metadata.

### Configuration

- [Settings](src/Settings.h): Common configurable settings (file name, path, etc.)
- [InputSettings](src/InputSettings.h): Configuration settings for any Input class.
- [InputADCSettings](src/InputADCSettings.h): Configuration settings for InputADC.
- [InputTDMSettings](src/InputTDMSettings.h): Configuration settings for InputTDM.
- [InputMenu](src/InputMenu.h): Actions and menu for checking and reading analog input data.
- [RTClockMenu](src/RTClcockMenu.h): Actions and menu for reading and setting the real-time clock.
- [SDCardMenu](src/SDCardMenu.h): Actions and menu for dealing with SD cards.
- [DiagnosticMenu](src/DiagnosticMenu.h): Actions and menu for diagnostics of the Teensy board, PSRAM memory, and devices.

### Audio monitor

- [AudioPlayBuffer](src/AudioPlayBuffer.h): Make the DataBuffer available as an input for the Audio library.
- [AudioMonitor](src/AudioMonitor.h): Play recorded data with optional feedback signals on speaker.

### Online analysis

- [AnalysisChain](src/AnalysisChain.h): Coordinate analysis of data snippets via Analyzer.
- [Analyzer](src/Analyzer.h): Base class for analyzers called by AnalysisChain.

### Utilities

- [Display](src/Display.h): Display data on a TFT monitor.
- [AllDisplays](src/AllDisplays.h): Include selected TFT library for the examples.

- [RTClock](src/RTClock.h): Time and date strings from the [onboard
  real time clock](https://www.pjrc.com/teensy/td_libs_Time.html).
- [RTClockDS1307](src/RTClockDS1307.h): Use external [DS1307, DS1337 and DS3231
  chips](https://www.pjrc.com/teensy/td_libs_DS1307RTC.html) real time clock.
- [Blink](src/Blink.h): Blinking LEDs.
- [PushButtons](src/PushButtons.h): Manage and query push buttons with callback functions.
- [DeviceID](src/DeviceID.h): Read out a device identifier.
- [TestSignals](src/TestSignals.h): Generate test signals on pulse-width modulation and DAC pins.
- [TeensyBoard](src/TeensyBoard.h): Find out which Teensy board we are running on.
- [TeeRecBanner](src/TeeRecBanner.h): ASCII art banner and TeeRec version for output streams.


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
- [sdtools](examples/sdtools): Inspect, test, and format SD cards.

### Demos

Demonstrate the usage of some of the provided libraries.

- [blink](examples/blink): Demonstrate usage of Blink class.
- [pushbuttons](examples/pushbuttons): Demonstrate usage of PushButtons class.
- [deviceid](examples/deviceid): Demonstrate usage of DeviceID class.


## Utilities

In [utils/](utils) you find some useful python scripts.

- [viewwave](utils/viewwave.py): display the traces in a wave file.
- [spectra](utils/spectra.py): power spectra of traces in wave files.
- [noise](utils/noise.py): plot and analyse baseline noise levels from wave files.
- [continuity](utils/continuity.py): check whether pulse signals recorded into wave file have consistent periods over many wave files.
- [mergechannels](utils/mergechannels.py): take from each provided wave file one channel and merge them into a single wav file.
- [cycles](utils/cycles.py): plot failures in pulse traces? - needs update.

For allowing these script to use metadata contained in the wav files
generated via the TeeRec library (pin names for channels, settings of
the ADC), install [audioio](https://github.com/janscience/audioio).


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
- [Frank's bat detector](https://forum.pjrc.com/threads/38988-Bat-detector)
- [Teensy Batdetector](https://github.com/CorBer/teensy_batdetector/releases/tag/v1.6)