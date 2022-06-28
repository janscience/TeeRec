# averaging

Find the optimal combination of averaging, conversion and sampling
speed resulting in the lowest noise levels for a given sampling rate,
resolution, and channel configuration.


## Dependencies

Install the [Watchdog
library](https://github.com/janelia-arduino/Watchdog) from Peter
Polidoro via Library Manager.


## Usage

1. Connect the analog input channels to a fixed voltage or short
   circuit them in your actual hardware configuration.
2. Connect the Teensy via USB to your computer and optionally insert
   an SD card.
3. Wait some time to the let the hardware settle in.
4. Open the averaging sketch in the Arduino IDE: File -> Examples ->
   TeeRec -> averaging.
5. Modify in the top section of the averaging sketch resolution,
   sampling rate, and channel configuration, as well as conversion and
   sampling speeds to check for, according to your requirements.
6. Open the serial monitor (Ctrl+Shift+M).
7. Then compile and run this sketch (Ctrl+U) until it tells you it
   finished (takes a while).

The sketch reports the standard deviation of the recorded raw integer
data for each setting and channel. The lower, the better. Ideally, the
standard deviations should be smaller than one. Choose from the table
the best settings und use them for your application.

Beware, the estimates of the standard deviations vary. Run the test
a few times to get an idea of the variability.


## Recorded data files

If an SD card was inserted, the sketch records for each combination of
averaging, conversion and sampling speed one data buffer to files,
stored in the `tests/` directory of the SD card. You may run the
[`extras/noise.py`](../../utils/noise.py) script on these files to
display histograms and to evaluate the noise levels in more detail.
