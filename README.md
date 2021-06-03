# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.5 microcontroller.

Based on Pedro Villanueva's [ADC](https://github.com/pedvide/ADC) library, 
Paul Stoffregen's [DMAChannel](https://github.com/PaulStoffregen/cores/tree/master/teensy3),
Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library, and on
Stefan Mucha's first [EOD logger sketch](https://github.com/muchaste/EOD-Logger/tree/master/eodlogger_v1) with contributions by [Lydia Federman](https://github.com/LydiaFe/EOD-Logger) and [Sebastian Volkmer](https://github.com/SebastianVol/EOD-Logger/blob/master/eodlogger_2channel_barebone/eodlogger_8channel.ino).


## Features

- Continuous DMA based data acquisition into a circular buffer.
- Single channel or multiplexed acquisition from multiple channels.
- Highspeed timed acquisition up to 500kHz.
- Acquisition from a single or both ADCs.
- Data are combined into a single multiplexed circular data buffer.
- Conversion of data to signed 16bit for direct storage into WAV files.
- Storing recorded data as wave files on SD cards.
- Display recorded data on a monitor.


## Libraries

- ContinousADC: Sample from multiple pins into a cyclic buffer.
- SDWrite: Write data on SD card.
- Display: Display data on a monitor.


## Examples

- scope: Show acquired data on a display.
- logger: Continuously store data on SD card.
- recorder: Show acquired data on a display and store data on SD card upon user request.
