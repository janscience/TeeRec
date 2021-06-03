# teerec - Teensy recorder

Libraries for recording analog input data on Teensy 3.5 microcontroller.


## Features

- Continuous DMA based data acquisition into a circular buffer.
- Single channel or multiplexed acquisition from multiple channels.
- Highspeed acquisition up to 500kHz.
- Acquisition from a single or both ADCs.
- Data are combined into a single multiplexed circular data buffer.
- Conversion of data to signed 16bit for direct storage into WAV files.
- Storing recorded data as wave files on SD cards.
- Display recorded data on a monitor.


## Libraries

- ContinousADC: Sample from multiple pins into a cyclic buffer.
- SDWrite: Write data on SD card.
- EFishMonitor: Display data on a monitor.


## Examples

- scope: Show acquired data on a display.
- logger: Continuously store data on SD card.
- recorder: Show acquired data on a display and store data on SD card upon user request.
