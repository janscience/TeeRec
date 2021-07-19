#include <Arduino.h>
#include <RTClock.h>


time_t getTeensy3Time() {
  return Teensy3Clock.get();
}


RTClock::RTClock() {
#ifdef DS1307RTC_h
  setSyncProvider(RTC.get);
#else
  setSyncProvider(getTeensy3Time);
#endif
}


bool RTClock::check() {
  bool status = (timeStatus() == timeSet);
  if (!status)
     Serial.println("Unable to sync with the RTC");
  return status;
}


void RTClock::date(char *str, bool brief) {
  str[0] = '\0';
  if (timeStatus() == timeNotSet)
    return;
  time_t t = now();
  if (brief)
    sprintf(str, "%04d%02d%02d", year(t), month(t), day(t));
  else
    sprintf(str, "%04d-%02d-%02d", year(t), month(t), day(t));
}


void RTClock::time(char *str, bool brief, bool dash) {
  str[0] = '\0';
  if (timeStatus() == timeNotSet)
    return;
  time_t t = now();
  if (brief)
    sprintf(str, "%02d%02d%02d", hour(t), minute(t), second(t));
  else if (dash)
    sprintf(str, "%02d-%02d-%02d", hour(t), minute(t), second(t));
  else
    sprintf(str, "%02d:%02d:%02d", hour(t), minute(t), second(t));
}


void RTClock::dateTime(char *str, bool brief, bool dash) {
  str[0] = '\0';
  if (timeStatus() == timeNotSet)
    return;
  time_t t = now();
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


String RTClock::makeStr(const String &str, bool dash) {
  char ts[20];
  String tstr = str;
  if (timeStatus() == timeNotSet)
    return "";
  if (tstr.indexOf("SDATETIME") >= 0) {
    dateTime(ts, true);
    tstr.replace("SDATETIME", ts);
  }
  else if (tstr.indexOf("DATETIME") >= 0) {
    dateTime(ts, false, dash);
    tstr.replace("DATETIME", ts);
  }
  else if (tstr.indexOf("SDATE") >= 0) {
    date(ts, true);
    tstr.replace("SDATE", ts);
  }
  else if (tstr.indexOf("DATE") >= 0) {
    date(ts, false);
    tstr.replace("DATE", ts);
  }
  else if (tstr.indexOf("STIME") >= 0) {
    time(ts, true);
    tstr.replace("STIME", ts);
  }
  else if (tstr.indexOf("TIME") >= 0) {
    time(ts, false, dash);
    tstr.replace("TIME", ts);
  }
  return tstr;
}


void RTClock::report() {
  char times[20];
  dateTime(times);
  Serial.println("RTC current time:");
  Serial.printf("  %s\n\n", times);
}
