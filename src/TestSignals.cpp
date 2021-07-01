#include <Arduino.h>
#include <TeensyBoard.h>
#include <TestSignals.h>


void testSignal(int pin, int frequency, float dc) {
  pinMode(pin, OUTPUT);
  analogWriteFrequency(pin, frequency);
  analogWrite(pin, int(dc*256));
}


void setupTestSignals(int *pins, int frequency) {
  Serial.println("Test signals:");
  // pins 0-10:
#if defined(TEENSY40) || defined(TEENSY41)
  int pintimer[MaxSignalPins] = {1, 2, 3, 3, 4, 5, 6, 7, 7, 6, 8};
#elif defined(TEENSY35) || defined(TEENSY36)
  int pintimer[MaxSignalPins] = {0, 0, 1, 2, 2, 3, 3, 1, 1, 3, 3};
#elif defined(TEENSY32)
  int pintimer[MaxSignalPins] = {0, 0, 0, 1, 1, 2, 2, 0, 0, 2, 2};
#else
  #error "Board not supported for PWM signal generation."
#endif
  // setup frequencies and duty cycles:
  const int maxtimers = 9;
  int timerfreqs[maxtimers] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  int pintimercount[maxtimers] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int maxdcs = 4;
  float pintimerdc[maxdcs] = {0.5, 0.2, 0.8, 0.33};  // duty cycle
  float maxfreqfac = 0.5;
  int freqs[MaxSignalPins];
  float dcs[MaxSignalPins];
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    int pin = pins[k];
    int timerid = pintimer[pin];
    if (timerid > 0) {
      if (timerfreqs[timerid] < 0) {
	maxfreqfac *= 2.0;
	timerfreqs[timerid] = maxfreqfac*frequency;
      }
      freqs[k] = timerfreqs[timerid];
      dcs[k] = pintimerdc[pintimercount[timerid]%maxdcs];
      pintimercount[timerid]++;
    }
    else {
      freqs[k] = -1;
      dcs[k] = -1.0;
    }
  }
  // activate pwm pins:
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    if (freqs[k] > 0)
      testSignal(pins[k], freqs[k], dcs[k]);
  }
  // report:
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    if (freqs[k] > 0)
      Serial.printf("  pin %2d: %5dHz, %3d%% duty cycle\n", pins[k], freqs[k], int(100.0*dcs[k]));
  }
  Serial.println();
}


void setupTestSignals(int pin0, int pin1, int frequency) {
  int n = pin1-pin0+1;
  int pins[n+1];
  for (int k=0; k<n; k++)
    pins[k] = pin0 + k;
  pins[n] = -1;
  setupTestSignals(pins, frequency);
}
