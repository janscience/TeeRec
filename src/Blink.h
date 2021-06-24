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

  void setTiming(uint32_t intervalms, uint32_t onms);

  void set(bool on);

  void update();


 protected:
  
  int Pin;
  bool Status;
  uint32_t OnTime;
  uint32_t OffTime;
  elapsedMillis Time;

};


#endif
