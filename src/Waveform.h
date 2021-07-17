/*
  Waveform - library for generating waveforms on DAC pins.
  Created by Jan Benda, July 16th, 2021.
*/

#ifndef Waveform_h
#define Waveform_h


#include <Arduino.h>


class Waveform {

 public:

  Waveform();
  ~Waveform();

  void setup(int pin, float rate);

  void start(float freq, float ampl=1.0);

  void stop();


 protected:

  static Waveform *WF;

  static void write();

  IntervalTimer Timer;

  int Pin;
  float Rate;
  volatile size_t NData;
  volatile uint16_t *Data;
  volatile size_t Index;

};


#endif
