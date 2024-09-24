/*
  RTClock - library for using a real-time clock.
  Created by Jan Benda, June 3rd, 2021.
  Based on https://github.com/PaulStoffregen/Time
  and on contributions from Stefan Mucha and Lydia Federman.
  See also https://www.pjrc.com/teensy/td_libs_Time.html 
  and https://www.pjrc.com/teensy/td_libs_DS1307RTC.html.

  If a DS1307, DS1337 or DS3231 chip is present, it is used as the
  source for getting real time.  Without such a chip the Teensy
  on-board real-time clock is used - you just need to attach a 3.3V
  battery to VBAT to make it work.
*/

#ifndef RTClock_h
#define RTClock_h


#include <Arduino.h>
#include <TimeLib.h>


class SDCard;


class RTClock {

 public:

  // Initialize the Teensy on-board real time clock.
  RTClock();

  // Set the source for real time clock to DS1307 chip if
  // present, otherwise the Teensy on-board real time clock is used.
  // Call this early on in setup() if you intend to use an DS1307 clock.
  void init();

  // Set the real time clock (either DS1307 if available or on-board)
  // via setSyncProvider() from TimeLib.h.
  // This function is called by the constructor or by init()
  // and you usually do not need to call it again.
  // You need to call it when waking up from a deep sleep, though.
  void setSync();

  // Check whether clock is available and set.
  // If not print message to stream.
  bool check(Stream &stream=Serial);

  // Set real-time clock to t.
  void set(time_t t);
  
  // Set real-time clock to the given time.
  // If from_start is true, the time the sketch is already running is
  // added to the specified time.
  // If check, then check if the given time is valid.
  // Return true if the time was successfully set.
  bool set(int year, int month, int day, int hour, int min, int sec,
	   bool from_start=false, bool check=true);

  // Set real-time clock to datetime string (YYYY-MM-DDThh:mm:ss).
  // If from_start is true, the time the sketch is already running is
  // added to datetime.
  // Return true if the time was successfully set.
  bool set(char *datetime, bool from_start=false);

  // Set real-time clock interactively from stream.
  // Return true if the time was successfully set.
  bool set(Stream &stream=Serial);

  // Set the current time from a file path on the sdcard.
  // If the file was found and the time set, the file is deleted.
  // If from_start is true, the time the sketch is already running is
  // added to the time found in the file to account for start-up delays.
  // Return true if the time was successfully set from the file.
  // The file contains a single line with the time in the format
  // YYYY-MM-DDTHH:MM:SS
  bool setFromFile(SDCard &sdcard, const char *path="settime.cfg",
		   bool from_start=true);

  // String with date in the format YYYY-MM-DD.
  // str needs to hold 11 characters.
  // If time equals zero, then get current time (`time = now()`).
  // If brief, without the dashes.
  void date(char *str, time_t time=0, bool brief=false) const;

  // String with time in the format HH:MM:SS.
  // str needs to hold 9 characters.
  // If time equals zero, then get current time (`time = now()`).
  // If brief, without the colons. If dash, replace colons by dashes.
  void time(char *str, time_t time=0, bool brief=false, bool dash=false) const;

  // String with date and time in the format YYYY-MM-DDTHH:MM:SS.
  // str needs to hold 20 characters.
  // If time equals zero, then get current time (`time = now()`).
  // If brief, without the dashes and colons. If dash, replace colons by dashes.
  void dateTime(char *str, time_t time=0,
		bool brief=false, bool dash=false) const;

  // Replace in str, DATE, TIME, DATETIME by the respective strings of time.
  // SDATE, STIME, SDATETIME are replaced by the corresponding brief
  // (short) versions.
  // If time equals zero, then get current time (`time = now()`).
  // If dash, replace colons by dashes.
  String makeStr(const String &str, time_t time=0, bool dash=false) const;

  // Write out current time, real time provider, and potential error message
  // on stream.
  void report(Stream &stream=Serial) const;

private:

  int RTCSource;

};


#endif
