/*
  RTClock - library for using a real-time clock.
  Created by Jan Benda, June 3rd, 2021.
  Based on https://github.com/PaulStoffregen/Time
  and on contributions from Stefan Mucha and Lydia Federman.

  By default the Teensy on-board real-time clock is used.

  If you want to use the DS1307RTC clock, then include
  DS1307RTC.h and call setSyncProvider(RTC.get) after
  instantiating RTClock.
*/

#ifndef RTClock_h
#define RTClock_h


#include <Arduino.h>
#include <TimeLib.h>


class RTClock {

 public:

  // Initialize.
  RTClock();

  // Check whether clock is available and set.
  bool check();

  // String with the current date in the format YYYY-MM-DD. str needs to hold 11 characters.
  // If brief, without the dashes.
  void date(char *str, bool brief=false);

  // String with the current time in the format HH:MM:SS. str needs to hold 9 characters.
  // If brief, without the colons. If dash, replace colons by dashes.
  void time(char *str, bool brief=false, bool dash=false);

  // String with the current date and time in the format YYYY-MM-DDTHH:MM:SS.
  // str needs to hold 20 characters.
  // If brief, without the dashes and colons. If dash, replace colons by dashes.
  void dateTime(char *str, bool brief=false, bool dash=false);

  // Replace in str, DATE, TIME, DATETIME by the respective strings of the current time.
  // SDATE, STIME, SDATETIME are replaced by the corresponding brief (short) versions.
  // If dash, replace colons by dashes.
  String makeStr(const String &str, bool dash=false);

  // Write out current time on serial.
  void report();

};


#endif
