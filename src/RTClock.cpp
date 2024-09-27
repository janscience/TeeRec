#include <Arduino.h>
#include <DS1307RTC.h>
#include <RTClock.h>
#include <SDWriter.h>


time_t getTeensyRTCTime() {
  return Teensy3Clock.get();
}


RTClock::RTClock() :
  RTCSource(0) {
  setSync();
}


void RTClock::init() {
  tmElements_t tm;
  RTCSource = RTC.read(tm) && RTC.chipPresent() ? 1 : 0;
  setSync();
}


void RTClock::setSync() {
  if (RTCSource == 1)
    setSyncProvider(RTC.get);
  else
    setSyncProvider(getTeensyRTCTime);
}


bool RTClock::check(Stream &stream) {
  bool status = (timeStatus() == timeSet);
  if (!status) {
    if (timeStatus() == timeNotSet)
      stream.println("RTC: time has never been set!");
    else if (timeStatus() == timeNeedsSync)
      stream.println("RTC: unable to sync time with RTC!");
  }
  return status;
}


void RTClock::set(time_t t) {
  setTime(t);
  if (RTCSource == 1)
    RTC.set(t);
  else
    Teensy3Clock.set(t);
}


bool RTClock::set(int year, int month, int day, int hour, int min, int sec,
		  bool from_start, bool check) {
  if (check) {
    if (year < 2020) {
      Serial.printf("Invalid year %d.\n", year);
      return false;
    }
    if (month < 1 || month > 12) {
      Serial.printf("Invalid month %d.\n", month);
      return false;
    }
    if (day < 1 || day > 31) {
      Serial.printf("Invalid day %d.\n", day);
      return false;
    }
    if (hour < 0 || hour > 23) {
      Serial.printf("Invalid hour %d.\n", hour);
      return false;
    }
    if (min < 0 || min > 59) {
      Serial.printf("Invalid minute %d.\n", min);
      return false;
    }
    if (sec < 0 || sec > 59) {
      Serial.printf("Invalid second %d.\n", sec);
      return false;
    }
  }
  // set time:
  tmElements_t tm;
  tm.Year = year - 1970;
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = min;
  tm.Second = sec;
  time_t t = makeTime(tm);
  if (from_start)
    t += millis()/1000;
  set(t);
  return true;
}


bool RTClock::set(char *datetime, bool from_start) {
  // parse date-time string YYYY-MM-DDThh:mm:ss
  int sepdt[6] = {4, 7, 10, 13, 16, 19};
  int nelements = 6;
  datetime[19] = '\0';
  for (int k=0; k<6; k++) {
    if (datetime[sepdt[k]] != '-' &&
	datetime[sepdt[k]] != 'T' &&
	datetime[sepdt[k]] != ':' &&
	datetime[sepdt[k]] != '\0') {
      nelements = k;
      break;
    }
  }
  if (nelements < 6) {
    if (nelements == 3) {
      // valid date string, add current time:
      char times[10];
      time(times);
      datetime[sepdt[2]] = 'T';
      strcpy(datetime + sepdt[2] + 1, times);
    }
    else {
      // parse time string hh:mm:ss
      int sept[3] = {2, 5, 8};
      nelements = 3;
      for (int k=0; k<3; k++) {
	if (datetime[sept[k]] != ':' &&
	    datetime[sept[k]] != '\0') {
	  nelements = k;
	  break;
	}
      }
      if (nelements == 3) {
	// valid time string, add current date:
	char dates[32];
	date(dates);
	dates[sepdt[2]] = 'T';
	strcpy(dates + sepdt[2] + 1, datetime);
	strcpy(datetime, dates);
      }
      else {
	Serial.printf("String \"%s\" is not a valid date-time string.\n", datetime);
	return false;
      }
    }
  }
  for (int k=0; k<6; k++)
    datetime[sepdt[k]] = '\0';
  int year = atoi(datetime);
  int month = atoi(&datetime[5]);
  int day = atoi(&datetime[8]);
  int hour = atoi(&datetime[11]);
  int min = atoi(&datetime[14]);
  int sec = atoi(&datetime[17]);
  return set(year, month, day, hour, min, sec, from_start, true);
}


bool RTClock::set(Stream &stream) {
  while (true) {
    stream.print("Enter date and time in ISO format (YYYY-MM-DDThh:mm:dd): ");
    while (stream.available() == 0)
      yield();
    char datetime[32];
    stream.readBytesUntil('\n', datetime, 32);
    stream.println(datetime);
    if (strlen(datetime) == 0 || datetime[0] == 'q')
      return false;
    memset(datetime + strlen(datetime), '\0', 32 - strlen(datetime));
    if (set(datetime)) {
      char times[20];
      dateTime(times);
      stream.printf("\nSuccessfully set real-time clock to: %s\n", times);
      return true;
    }
  }
}


bool RTClock::setFromFile(SDCard &sdcard, const char *path, bool from_start) {
  FsFile file = sdcard.openRead(path);
  if (!file)
    return false;
  char datetime[20];
  file.read(datetime, sizeof(datetime));
  file.close();
  bool r = set(datetime, from_start);
  if (! r)
    return false;
  sdcard.remove(path);
  // get time:
  char times[20];
  dateTime(times);
  Serial.printf("Set real time clock from file \"%s\" to %s.\n\n",
		path, times);
  return true;
}


void RTClock::date(char *str, time_t t, bool brief) const {
  str[0] = '\0';
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return;
    t = now();
  }
  if (brief)
    sprintf(str, "%04d%02d%02d", year(t), month(t), day(t));
  else
    sprintf(str, "%04d-%02d-%02d", year(t), month(t), day(t));
}


void RTClock::time(char *str, time_t t, bool brief, bool dash) const {
  str[0] = '\0';
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return;
    t = now();
  }
  if (brief)
    sprintf(str, "%02d%02d%02d", hour(t), minute(t), second(t));
  else if (dash)
    sprintf(str, "%02d-%02d-%02d", hour(t), minute(t), second(t));
  else
    sprintf(str, "%02d:%02d:%02d", hour(t), minute(t), second(t));
}


void RTClock::dateTime(char *str, time_t t, bool brief, bool dash) const {
  str[0] = '\0';
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return;
    t = now();
  }
  if (brief)
    sprintf(str, "%04d%02d%02dT%02d%02d%02d",
	    year(t), month(t), day(t), hour(t), minute(t), second(t));
  else if (dash)
    sprintf(str, "%04d-%02d-%02dT%02d-%02d-%02d",
	    year(t), month(t), day(t), hour(t), minute(t), second(t));
  else
    sprintf(str, "%04d-%02d-%02dT%02d:%02d:%02d",
	    year(t), month(t), day(t), hour(t), minute(t), second(t));
}


String RTClock::makeStr(const String &str, time_t t, bool dash) const {
  char ts[20];
  String tstr = str;
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return "";
    t = now();
  }
  if (tstr.indexOf("SDATETIME") >= 0) {
    dateTime(ts, t, true);
    tstr.replace("SDATETIME", ts);
  }
  else if (tstr.indexOf("DATETIME") >= 0) {
    dateTime(ts, t, false, dash);
    tstr.replace("DATETIME", ts);
  }
  else if (tstr.indexOf("SDATE") >= 0) {
    date(ts, t, true);
    tstr.replace("SDATE", ts);
  }
  else if (tstr.indexOf("DATE") >= 0) {
    date(ts, t, false);
    tstr.replace("DATE", ts);
  }
  else if (tstr.indexOf("STIME") >= 0) {
    time(ts, t, true);
    tstr.replace("STIME", ts);
  }
  else if (tstr.indexOf("TIME") >= 0) {
    time(ts, t, false, dash);
    tstr.replace("TIME", ts);
  }
  return tstr;
}


void RTClock::report(Stream &stream) const {
  char source[22];
  if (RTCSource == 1)
    strcpy(source, "DS1307/DS1337/DS3231");
  else
    strcpy(source, "on-board");
  char times[20];
  dateTime(times);
  stream.printf("RTC (%s) current time: %s\n", source, times);
  if (timeStatus() != timeSet) {
    if (timeStatus() == timeNotSet)
      stream.println("  time has never been set!");
    else if (timeStatus() == timeNeedsSync)
      stream.println("  unable to sync time with RTC!");
  }
  stream.println();
}

