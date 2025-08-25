/*
  Blink - library for blinking LEDs.
  Created by Jan Benda, June 24th, 2021.
*/

#ifndef Blink_h
#define Blink_h


#include <Arduino.h>
#include <DigitalIODevice.h>


class Blink {

 public:

  // Initialize Blink. At some point you need to specify the pin to be
  // used for controling a LED via setPin().
  Blink(const char *name);

  // Control LEDs on internal pin1 and/or pin2. If invert, LOW is on.
  Blink(const char *name, int pin1, bool invert1=false,
	int pin2=-1, bool invert2=false);

  // Control LEDs on internal pin1 and pin2 on a digital IO device.
  // If invert, LOW is on.
  Blink(const char *name, int pin1, bool invert1,
	DigitalIODevice &dev2, int pin2, bool invert2=false);

  // Control LEDs on pin on a digital IO device. If invert, LOW is on.
  Blink(const char *name, DigitalIODevice &dev, int pin, bool invert=false);

  // Control LEDs on pin1 and pin2 on digital IO devices. If invert, LOW is on.
  Blink(const char *name, DigitalIODevice &dev1, int pin1, bool invert1,
	DigitalIODevice &dev2, int pin2, bool invert2=false);

  // Switch off LED.
  ~Blink();

  // Name of this Blink instance.
  const char *name() const { return Name; };

  // Set pin for an LED on an internal pin of the microcontroller.
  // If invert, LOW is on.
  // Returns the index of the pin. On error -1 is returned.
  int8_t setPin(uint8_t pin=LED_BUILTIN, bool invert=false);

  // Set pin for an LED on a digital IO device if it is available.
  // If invert, LOW is on.
  // Returns the index of the pin. On error -1 is returned.
  int8_t setPin(DigitalIODevice &device, uint8_t pin, bool invert=false);

  // Remove all pins.
  void clearPins();

  // Enable pin.
  void enablePin(uint8_t index, bool enable=true);

  // Switch LED on this pin off and disable it.
  void disablePin(uint8_t index);

  // True if at least one pin is controlled by this. 
  bool available() const { return NPins > 0; };

  // Report digital IO devices and pins used for blinking on stream.
  void report(Stream &stream=Serial);

  // Reset pin configuration
  // (in case the pin mode was reconfigured by something else).
  void reset();

  
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
  // As long not allow_on, do not switch LEDs on.
  void update(bool allow_on=true);

  // Delay sketch by delayms milliseconds while blinking.
  void delay(uint32_t delayms);

  // Retrieve the stored switch times and states.
  // times and states must hold MaxTimes elements.
  // Returns true if the buffers overflowed.
  bool getSwitchTimes(uint32_t *times, bool *states, size_t *n);

  // Return the number of stored switch times.
  size_t nswitchTimes() const { return NSwitchTimes; };

  // Clear the buffer holding switch times.
  void clearSwitchTimes();

  
  // basic level:
  
  // Manually switch LED on or off.
  void switchOn(bool on=true);
  
  // Manually switch LED off.
  void switchOff();

  // Return true if LEDs are currently switched on.
  bool isOn() const;

  // A random number between 0 and 1.
  static float urand(void);
  
  static const int MaxTimes = 32;

  
 protected:

  static const size_t MaxName = 16;
  char Name[MaxName];

  static DigitalIODevice *InternIOs;

  static const uint8_t MaxPins = 4;
  DigitalIODevice *Devices[MaxPins];
  uint8_t Pins[MaxPins];
  bool Enabled[MaxPins];
  uint8_t NPins;
  bool On;
  uint32_t Times[2][MaxTimes];
  uint32_t  Delay;
  bool Random;
  int Index;
  int State;
  elapsedMillis Time;

  uint32_t Interval;
  uint32_t OnTime;
  uint32_t OffTime;

  uint32_t SwitchTimes[MaxTimes];
  bool SwitchStates[MaxTimes];
  size_t NSwitchTimes;
  bool SwitchOverflow;

  static volatile uint64_t PRNGState;  // any nonzero state is valid
  
};


#endif
