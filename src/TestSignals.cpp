#include <Arduino.h>
#include <TeensyBoard.h>
#include <TestSignals.h>


void testSignal(int pin, int frequency, float dc) {
  pinMode(pin, OUTPUT);
  analogWriteFrequency(pin, frequency);
  analogWrite(pin, int(dc*256));
}


void setupTestSignals(int pin0, int pin1, int frequency) {
#if defined(TEENSY35) || defined(TEENSY36)
  // Teensy 3.5 & 3.6, pins with same frequency:
  int pinfreqs[] = {0, 0, 1, 2, 2, 3, 3, 1, 1, 3, 3};
#endif
  int pinfreqcount[] = {0, 0, 0, 0};
  float pinfreqdc[] = {0.5, 0.2, 0.8, 0.33};
  for (int pin=pin0; pin<=pin1; pin++) {
    int freqi = pinfreqs[pin];
    testSignal(pin, frequency*freqi, pinfreqdc[pinfreqcount[freqi]]);
    pinfreqcount[freqi]++;
  }
}

