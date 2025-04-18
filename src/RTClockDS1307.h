/*
  RTClockDS1307 - using an DS1307, DS1337 or DS3231 real-time clock.
  Created by Jan Benda, January 17th, 2025.
  Based on https://github.com/PaulStoffregen/Time
  and on contributions from Stefan Mucha and Lydia Federman.
  See also https://www.pjrc.com/teensy/td_libs_Time.html 
  and https://www.pjrc.com/teensy/td_libs_DS1307RTC.html.

  See examples/logger/logger.ino for an example.
*/

#ifndef RTClockDS1307_h
#define RTClockDS1307_h


#include <RTClock.h>


class RTClockDS1307 : public RTClock {

 public:

  // Initialize the Teensy on-board real time clock.
  RTClockDS1307();

  // Set the source for real time clock to the DS1307, DS1337 or
  // DS3231 chip if present, otherwise the Teensy on-board real time
  // clock is used.
  // Return true if an DS1307, DS1337 or DS3231 clock is available.
  // Call this early on in setup() if you intend to use an DS1307 clock.
  virtual bool begin();

  // Set real-time clock to t.
  void set(time_t t);

};


#endif
