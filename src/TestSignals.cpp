#include <Arduino.h>
#include <TeensyBoard.h>
#include <TestSignals.h>


void testSignal(int pin, int frequency, float dc) {
  pinMode(pin, OUTPUT);
  analogWriteFrequency(pin, frequency);
  analogWrite(pin, int(dc*256));
}


void setupTestSignals(int *pins, int frequency) {
  // pins 0-10 with same timer:
#if defined(TEENSY35) || defined(TEENSY36)
  int pintimer[MaxSignalPins] = {0, 0, 1, 2, 2, 3, 3, 1, 1, 3, 3};
#elif defined(TEENSY40) || defined(TEENSY41)
  // Teensy 3.5 & 3.6, pins 0-10 with same frequency:
  int pintimer[MaxSignalPins] = {1, 2, 3, 3, 4, 5, 6, 7, 7, 6, 8};
#else
  #error "Board not supported for PWM signal generation."
#endif
  int timerfreqs[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  int pintimercount[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  float pintimerdc[] = {0.5, 0.2, 0.8, 0.33};  // duty cycle
  float maxfreqfac = 0.5;
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    int pin = pins[k];
    int timerid = pintimer[pin];
    if (timerid > 0) {
      if (timerfreqs[timerid] < 0) {
	maxfreqfac *= 2.0;
	timerfreqs[timerid] = maxfreqfac*frequency;
      }
      testSignal(pin, timerfreqs[timerid], pintimerdc[pintimercount[timerid]]);
      pintimercount[timerid]++;
    }
  }
}


void setupTestSignals(int pin0, int pin1, int frequency) {
  int n = pin1-pin0+1;
  int pins[n+1];
  for (int k=0; k<n; k++)
    pins[k] = pin0 + k;
  pins[n] = -1;
  setupTestSignals(pins, frequency);
}
