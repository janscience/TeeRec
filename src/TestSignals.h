/*
  TestSignals - generate test signals on pulse-width modulation and DAC pins.
  Created by Jan Benda, June 4th, 2021.
*/

#ifndef TestSignals_h
#define TestSignals_h


#include <Arduino.h>


// Start generating PWM signal with frequency in Hertz and duty cycle dc (0 to 1)
// on a pin.
void testSignal(int pin, int frequency, float dc);

// Maximum number of signal pins.
static const int MaxSignalPins = 11;

// Start generating PWM singals on the specified pins (-1 terminated)
// with multiples of frequency and various duty cycles.
void setupTestSignals(int *pins, int frequency);

// Start generating PWM singals on pin0 to pin1 inclusively with
// multiples of frequency and various duty cycles.
void setupTestSignals(int pin0, int pin1, int frequency);


// Waveform generation on DAC pins.
class Waveform {

 public:

  // Initialize waveform generator with 100kHz sampling rate.
  Waveform();
  ~Waveform();

  // Initialize waveform generator with sampling rate.
  void setup(float rate);

  // Start generating a signal on the specified DAC pin,
  // fundamental frequency and amplitude (0-1).
  // The signal is generated between pin and GND right next to it.
  void start(int pin, float freq, float ampl=1.0);

  // Stop generating a signal.
  void stop();


 protected:

  static Waveform *WF;

  static void write();

  IntervalTimer Timer;

  float Rate;
  uint16_t MaxValue;
  volatile int Pin;
  volatile size_t NData;
  volatile uint16_t *Data;
  volatile size_t Index;

};


#endif
