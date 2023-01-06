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


bool RTClock::check() {
  bool status = (timeStatus() == timeSet);
  if (!status) {
    if (timeStatus() == timeNotSet)
      Serial.println("RTC: time has never been set!");
    else if (timeStatus() == timeNeedsSync)
      Serial.println("RTC: unable to sync time with RTC!");
  }
  return status;
}


bool RTClock::setFromFile(SDCard &sdcard, const char *path, bool from_start) {
  File file = sdcard.openRead(path);
  if (!file)
    return false;
  char datetime[20];
  file.read(datetime, sizeof(datetime));
  file.close();
  // parse date-time string YYYY-MM-DDTHH:MM:SS
  int sepi[6] = {4, 7, 10, 13, 16, 19};
  datetime[19] = '\0';
  for (int k=0; k<6; k++) {
    if (datetime[sepi[k]] != '-' &&
	datetime[sepi[k]] != 'T' &&
	datetime[sepi[k]] != ':' &&
	datetime[sepi[k]] != '\0') {
      Serial.printf("File \"%s\" does not contain a valid date-time string.\n", path);
      return false;
    }
    else
      datetime[sepi[k]] = '\0';
  }
  int year = atoi(datetime);
  int month = atoi(&datetime[5]);
  int day = atoi(&datetime[8]);
  int hour = atoi(&datetime[11]);
  int min = atoi(&datetime[14]);
  int sec = atoi(&datetime[17]);
  if (year < 2020) {
    Serial.printf("Invalid year \"%s\" in file \"%s\".\n", datetime, path);
    return false;
  }
  if (month < 1 || month > 12) {
    Serial.printf("Invalid month \"%s\" in file \"%s\".\n", &datetime[5], path);
    return false;
  }
  if (day < 1 || day > 31) {
    Serial.printf("Invalid day \"%s\" in file \"%s\".\n", &datetime[8], path);
    return false;
  }
  if (hour < 0 || hour > 23) {
    Serial.printf("Invalid hour \"%s\" in file \"%s\".\n", &datetime[11], path);
    return false;
  }
  if (min < 0 || min > 59) {
    Serial.printf("Invalid minute \"%s\" in file \"%s\".\n", &datetime[14], path);
    return false;
  }
  if (sec < 0 || sec > 59) {
    Serial.printf("Invalid second \"%s\" in file \"%s\".\n", &datetime[17], path);
    return false;
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
  setTime(t);
  if (RTCSource == 1)
    RTC.set(t);
  else
    Teensy3Clock.set(t);
  sdcard.remove(path);
  // get time:
  char times[20];
  dateTime(times);
  Serial.printf("Set real time clock from file \"%s\" to %s.\n\n",
		path, times);
  return true;
}


void RTClock::date(char *str, time_t t, bool brief) {
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


void RTClock::time(char *str, time_t t, bool brief, bool dash) {
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


void RTClock::dateTime(char *str, time_t t, bool brief, bool dash) {
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


String RTClock::makeStr(const String &str, time_t t, bool dash) {
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


void RTClock::report() {
  char source[10];
  if (RTCSource == 1)
    strcpy(source, "DS1307");
  else
    strcpy(source, "on-board");
  char times[20];
  dateTime(times);
  Serial.printf("RTC (%s) current time: %s\n", source, times);
  if (timeStatus() != timeSet) {
    if (timeStatus() == timeNotSet)
      Serial.println("  time has never been set!");
    else if (timeStatus() == timeNeedsSync)
      Serial.println("  unable to sync time with RTC!");
  }
  Serial.println();
}
