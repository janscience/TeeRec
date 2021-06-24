/*
  Blink - library for blinking LEDs.
  Created by Jan Benda, June 24th, 2021.
*/

#ifndef Blink_h
#define Blink_h


#include <Arduino.h>


class Blink {

 public:

  Blink(int pin=LED_BUILTIN);
  ~Blink();

  // Set simple blink interval. Every intervalms the LED is on for onms.
  void set(uint32_t intervalms, uint32_t onms);

  // Set arbitrary blink intervals that repeat periodically.
  // First interval is LED on. Null terminated.
  void set(const uint32_t *times);

  // One shot sequence of blink intervals.
  // First interval is LED on. Null terminated.
  // After finishing fall back to the default blinking defined by set().
  void blink(const uint32_t *times);
  
  void switchOn(bool on=true);
  
  void switchOff();

  void update();


 protected:
  
  int Pin;
  bool On;
  uint32_t *Times[2];
  int Index;
  int State;
  elapsedMillis Time;

};


#endif
