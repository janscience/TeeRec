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
  // used for controling a LED via setPin() or setPin2().
  Blink();

  // Control LEDs on pin1 and/or pin2. If invert, LOW is on.
  Blink(int pin1, bool invert1=false, int pin2=-1, bool invert2=false);

  // Switch off LED.
  ~Blink();

  // Set pin of primary LED. If invert, LOW is on.
  void setPin(int pin=LED_BUILTIN, bool invert=false);

  // Set pin of secondary LED. If invert, LOW is on.
  void setPin2(int pin=LED_BUILTIN, bool invert=false);

  
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
  void setSingle(bool reset=true);

  // Set a blinking pattern with a double blink, based on the timings
  // provided by setTiming().
  void setDouble(bool reset=true);

  // Set a blinking pattern with a triple blink, based on the timings
  // provided by setTiming().
  void setTriple(bool reset=true);

  // Set a blinking pattern with a n blinks separated by a longer pause,
  // based on the timings provided by setTiming().
  void setMultiple(int n, bool reset=true);

  // Set a andom blinking pattern with on and off times uniformly
  // distributed between OnTime and OffTime.
  void setRandom(bool reset=true);

  // One-shot single blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this one-time single blink, the blinking
  // pattern provided by setSingle(), setDouble(), setTriple(),
  // setMultiple(), set() or setDelayed() is resumed.
  void blinkSingle(uint32_t intervalms=0, uint32_t onms=0, bool reset=true);

  // One-shot double blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this one-time double blink, the blinking
  // pattern provided by setSingle(), setDouble(), setTriple(),
  // setMultiple(), set() or setDelayed() is resumed.
  void blinkDouble(uint32_t intervalms=0, uint32_t onms=0, uint32_t offms=0,
		   bool reset=true);

  // One-shot triple blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this one-time triple blink, the blinking pattern
  // provided by setSingle(), setDouble(), setTriple(), setMultiple(),
  // set() or setDelayed() is resumed.
  void blinkTriple(uint32_t intervalms=0, uint32_t onms=0, uint32_t offms=0,
		   bool reset=true);
 
  // One-shot multiple blink, based on the timings provided by
  // setTiming(). Arguments larger than zero overwrite settings from
  // setTiming(). After this one-time multiple blink, the blinking
  // pattern provided by setSingle(), setDouble(), setTriple(),
  // setMultiple(), set() or setDelayed() is resumed.
  void blinkMultiple(int n, uint32_t intervalms, uint32_t onms,
		     uint32_t offms, bool reset=true);

  
  // detailed level (requires update() to be called regularly):

  // Set simple blink interval. Every intervalms the LED is on for onms.
  void set(uint32_t intervalms, uint32_t onms, bool reset=true);

  // Set arbitrary blink intervals that repeat periodically.
  // First interval is LED on. Null terminated.
  void set(const uint32_t *times, bool reset=true);

  // Set simple blink interval. Every intervalms the LED is on for onms,
  // starting after a delay of delayms milliseconds.
  void setDelayed(uint32_t delayms, uint32_t intervalms, uint32_t onms,
		  bool reset=true);

  // Set arbitrary blink intervals that repeat periodically,
  // starting after a delay of delayms milliseconds.
  // First interval is LED on. Null terminated.
  void setDelayed(uint32_t delayms, const uint32_t *times,
		  bool reset=true);
  
  // One shot simple blink interval. The LED is on for onms.
  // After intervalms fall back to the default blinking defined by set().
  void blink(uint32_t intervalms, uint32_t onms, bool reset=true);

  // One shot sequence of blink intervals.
  // First interval is LED on. Null terminated.
  // After finishing fall back to the default blinking defined by set().
  void blink(const uint32_t *times, bool reset=true);
  
  // Switch off all blinking and the LED.
  void clear();


  // execute the blinking:
  
  // Orchestrate all the blinking set by the above functions.
  // Call this function as often as possible in your loop().
  void update();

  // Delay sketch by delayms milliseconds while blinking.
  void delay(uint32_t delayms);

  // Return the time when the LED was last switched on in milliseconds.
  // When called again and the LED has not been switched on again, 0 is returned.
  uint32_t switchedOnTime() const;

  // Return the time when the LED was last switched off in milliseconds.
  // When called again and the LED has not been switched off again, 0 is returned.
  uint32_t switchedOffTime() const;

  
  // basic level:
  
  // Manually switch LED on or off.
  void switchOn(bool on=true);
  
  // Manually switch LED off.
  void switchOff();

  // A random number between 0 and 1.
  static float urand(void);
  
  
 protected:
  
  int Pin1;
  int Pin2;
  bool Invert1;
  bool Invert2;
  bool On;
  static const int MaxTimes = 32;
  uint32_t Times[2][MaxTimes];
  uint32_t  Delay;
  bool Random;
  int Index;
  int State;
  elapsedMillis Time;

  uint32_t Interval;
  uint32_t OnTime;
  uint32_t OffTime;

  mutable uint32_t LastOn;
  mutable uint32_t LastOff;

  static volatile uint64_t PRNGState;  // any nonzero state is valid
  
};


#endif
