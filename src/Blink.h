/*
  Blink - library for blinking LEDs.
  Created by Jan Benda, June 24th, 2021.
*/

#ifndef Blink_h
#define Blink_h


#include <Arduino.h>


class Blink {

 public:

  // Initialize Blink. At some point you need to specify the pin to be
  // used for controling a LED via setPin().
  Blink();

  // Control LED on pin. If invert, LOW is on.
  Blink(int pin, bool invert=false);

  // Switch off LED.
  ~Blink();

  // Set pin of LED. If invert, LOW is on.
  void setPin(int pin=LED_BUILTIN, bool invert=false);

  // abstract level (requires update() to be called regularly):

  // Set interval, on time, and off time in milliseconds.
  // Defaults are 2000, 50, and 150 milliseconds, respectively.
  void setTiming(uint32_t intervalms, uint32_t onms=50, uint32_t offms=150);

  // Return the interval used for automatic blink patterns in milliseconds.
  uint32_t interval() const { return Interval; };

  // Return the on time used for automatic blink patterns in milliseconds.
  uint32_t ontime() const { return OnTime; };

  // Return the off time used for automatic blink patterns in milliseconds.
  uint32_t offtime() const { return OffTime; };

  // Set a blinking pattern with a single blink, based on the timings
  // provided by setTiming().
  void setSingle();

  // Set a blinking pattern with a double blink, based on the timings
  // provided by setTiming().
  void setDouble();

  // Set a blinking pattern with a triple blink, based on the timings
  // provided by setTiming().
  void setTriple();

  // One-shot single blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this single blink, the blinking pattern
  // provided by setSingle(), setDouble(), setTriple(), set() or
  // setDelayed() is resumed.
  void blinkSingle(uint32_t intervalms=0, uint32_t onms=0);

  // One-shot double blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this single blink, the blinking pattern
  // provided by setSingle(), setDouble(), setTriple(), set() or
  // setDelayed() is resumed.
  void blinkDouble(uint32_t intervalms=0, uint32_t onms=0, uint32_t offms=0);

  // One-shot triple blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this single blink, the blinking pattern
  // provided by setSingle(), setDouble(), setTriple(), set() or
  // setDelayed() is resumed.
  void blinkTriple(uint32_t intervalms=0, uint32_t onms=0, uint32_t offms=0);
 

  // detailed level (requires update() to be called regularly):

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
  
  // One shot simple blink interval. The LED is on for onms.
  // After intervalms fall back to the default blinking defined by set().
  void blink(uint32_t intervalms, uint32_t onms);

  // One shot sequence of blink intervals.
  // First interval is LED on. Null terminated.
  // After finishing fall back to the default blinking defined by set().
  void blink(const uint32_t *times);

  // Switch off all blinking and the LED.
  void clear();

  // Call this function as often as possible in your loop().
  void update();

  // Delay sketch by delayms milliseconds while blinking.
  void delay(uint32_t delayms);


  // basic level:
  
  // Manually switch LED on or off.
  void switchOn(bool on=true);
  
  // Manually switch LED off.
  void switchOff();

  
 protected:
  
  int Pin;
  bool Invert;
  bool On;
  static const int MaxTimes = 11;
  uint32_t Times[2][MaxTimes];
  uint32_t  Delay;
  int Index;
  int State;
  elapsedMillis Time;

  uint32_t Interval;
  uint32_t OnTime;
  uint32_t OffTime;
};


#endif
