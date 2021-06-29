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

  // Set simple blink interval. Every intervalms the LED is on for onms,
  // starting after a delay of delayms milliseconds.
  void setDelayed(uint32_t delayms, uint32_t intervalms, uint32_t onms);

  // Set arbitrary blink intervals that repeat periodically,
  // starting after a delay of delayms milliseconds.
  // First interval is LED on. Null terminated.
  void setDelayed(uint32_t delayms, const uint32_t *times);

  // Switch off all blinking.
  void clear();
  
  // One shot simple blink interval. Within intervalms the LED is on for onms.
  // After finishing fall back to the default blinking defined by set().
  void blink(uint32_t intervalms, uint32_t onms);

  // One shot sequence of blink intervals.
  // First interval is LED on. Null terminated.
  // After finishing fall back to the default blinking defined by set().
  void blink(const uint32_t *times);

  // Manually switch LED on or off.
  void switchOn(bool on=true);
  
  // Manually switch LED off.
  void switchOff();

  // Call this function as often as possible in your loop().
  void update();


 protected:
  
  int Pin;
  bool On;
  static const int MaxTimes = 11;
  uint32_t Times[2][MaxTimes];
  uint32_t  Delay;
  int Index;
  int State;
  elapsedMillis Time;

};


#endif
