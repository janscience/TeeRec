/*
  RTClock - base class for using a real-time clock.
  Created by Jan Benda, June 3rd, 2021.
  Based on https://github.com/PaulStoffregen/Time
  and on contributions from Stefan Mucha and Lydia Federman.
  See also https://www.pjrc.com/teensy/td_libs_Time.html.

  Uses the Teensy on-board real-time clock. You just need to attach a
  3.3V battery to VBAT to make it work.
  
  For using a DS1307, DS1337 DS3231, or MAX31328 chip,
  see RTClockDS1307.h.
*/

#ifndef RTClock_h
#define RTClock_h


#include <Arduino.h>
#include <TimeLib.h>
#include <Device.h>


class SDCard;


class RTClock : public Device {

 public:

  // Initialize usage of the Teensy on-board real time clock.
  RTClock();

  // Set the source to an external real time clock.
  // Return true on success.
  // The default implementation return true.
  virtual bool begin();

  // Check whether clock is available and set.
  // If not print message to stream.
  bool check(Stream &stream=Serial);

  // Set real-time clock to time t.
  virtual void set(time_t t);
  
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

  // Set real-time clock interactively from streams.
  // Return true if the time was successfully set.
  bool set(Stream &instream=Serial, Stream &outstream=Serial);

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
  // If not seconds, just print hours and minutes without seconds
  // in the format HH:MM
  void time(char *str, time_t time=0, bool brief=false,
	    bool dash=false, bool seconds=true) const;

  // String with date and time in the format YYYY-MM-DDTHH:MM:SS.
  // str needs to hold 20 characters.
  // If time equals zero, then get current time (`time = now()`).
  // If brief, without the dashes and colons. If dash, replace colons by dashes.
  // If not seconds, just print hours and minutes without seconds
  // in the format YYYY-MM-DDTHH:MM
  void dateTime(char *str, time_t time=0, bool brief=false,
		bool dash=false, bool seconds=true) const;

  // Replace in str, DATE, TIME, TIMEM, DATETIME, DATETIMEM by the
  // respective strings of date and time.
  // SDATE, STIME, STIMEM, SDATETIME, SDATETIMEM are replaced by the
  // corresponding brief (short) versions without dashes or colons.
  // TIME is HH:MM:SS, TIMEM is HH:MM, likewise for DATETIME and DATETIMEM.
  // If time equals zero, then get current time (`time = now()`).
  // If dash, replace colons by dashes.
  String makeStr(const String &str, time_t time=0, bool dash=false) const;

  // Print current time on stream.
  void print(Stream &stream=Serial) const;

  // Print current time, real time provider, and potential error
  // message on stream.
  virtual void write(Stream &stream=Serial, size_t indent=0,
		     size_t indent_incr=4) const;

};


#endif
