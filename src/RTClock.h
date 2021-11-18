/*
  RTClock - library for using a real-time clock.
  Created by Jan Benda, June 3rd, 2021.
  Based on https://github.com/PaulStoffregen/Time
  and on contributions from Stefan Mucha and Lydia Federman.
  See also https://www.pjrc.com/teensy/td_libs_Time.html .

  If a DS1307 chip is present, it is used as the source for getting
  real time.  Without such a chip the Teensy on-board real-time clock
  is used - you just need to attach a 3V battery to VBAT to make it
  work.
*/

#ifndef RTClock_h
#define RTClock_h


#include <Arduino.h>
#include <TimeLib.h>


class SDCard;


class RTClock {

 public:

  // Initialize.  Set the source for real time clock to DS1307 chip if
  // present, otherwise the Teensy on-board real time clock is used.
  RTClock();

  // Check whether clock is available and set.
  // If not print message to serial.
  bool check();

  // Set the current time from a file path on the sdcard.
  // If the file was found and the time set, the file is deleted.
  // If from_start is true, the time the sketch is already running is
  // added to the time found in the file to account for start-up delays.
  // Return true if the time was set from the file.
  // The file contains a single line with the time in the format
  // YYYY-MM-DDTHH:MM:SS
  bool setFromFile(SDCard &sdcard, const char *path="settime.cfg",
		   bool from_start=true);

  // String with the current date in the format YYYY-MM-DD.
  // str needs to hold 11 characters.
  // If brief, without the dashes.
  void date(char *str, bool brief=false);

  // String with the current time in the format HH:MM:SS.
  // str needs to hold 9 characters.
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

  // Write out current time, real time provider, and potential error message
  // on serial.
  void report();

private:

  int RTCSource;

};


#endif
