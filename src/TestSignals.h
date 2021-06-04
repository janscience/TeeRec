/*
  TestSignals - generate test signals on pulse-width modulation pins.
  Created by Jan Benda, June 4th, 2021.
*/

#ifndef TestSignals_h
#define TestSignals_h


#include <Arduino.h>


// Start generating PWM signal with frequency in Hertz and duty cycle dc (0 to 1)
// on a pin.
void testSignal(int pin, int frequency, float dc);

// Start generating PWM singals on pin0 to pin1 inclusively with
// multiples of frequency and various duty cycles.
void setupTestSignals(int pin0, int pin1, int frequency);

#endif
