#include <Arduino.h>
#include <RTClock.h>
#include <SDCard.h>


time_t getTeensyRTCTime() {
  return Teensy3Clock.get();
}


RTClock::RTClock() :
  Device(),
  GetTime(0) {
  setDeviceType("clock");
  setInternBus();
  setChip("Teensy-RTC");
  setSyncProvider(getTeensyRTCTime);
  GetTime = getTeensyRTCTime;
  Available = true;
}


bool RTClock::begin() {
  return true;
}


bool RTClock::check(Stream &stream) {
  bool status = (timeStatus() == timeSet);
  if (!status) {
    if (timeStatus() == timeNotSet)
      stream.println("RTC: time has never been set!");
    else if (timeStatus() == timeNeedsSync)
      stream.println("RTC: time needs to be synchronized with RTC!");
  }
  return status;
}


time_t RTClock::checkTime(int year, int month, int day,
			  int hour, int min, int sec,
			  bool check) {
  time_t t = 0;
  if (check) {
    if (year < 2020) {
      Serial.printf("Invalid year %d.\n", year);
      return t;
    }
    if (month < 1 || month > 12) {
      Serial.printf("Invalid month %d.\n", month);
      return t;
    }
    if (day < 1 || day > 31) {
      Serial.printf("Invalid day %d.\n", day);
      return t;
    }
    if (hour < 0 || hour > 23) {
      Serial.printf("Invalid hour %d.\n", hour);
      return t;
    }
    if (min < 0 || min > 59) {
      Serial.printf("Invalid minute %d.\n", min);
      return t;
    }
    if (sec < 0 || sec > 59) {
      Serial.printf("Invalid second %d.\n", sec);
      return t;
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
  return makeTime(tm);
}


bool parseTimeStr(const char *time, tmElements_t &tm) {
  // parse:
  char vstr[8] = "";
  unsigned int vk = 0;
  int values[3] = {-1, 0, 0};
  unsigned int n = 0;
  bool success = true;
  for (unsigned int k=0; k <= strlen(time); k++) {
    if (k == strlen(time) || time[k] == ':') {
      vstr[vk] = '\0';
      if (strlen(vstr) > 0)
	values[n++] = atoi(vstr);
      vk = 0;
    }
    else if (isdigit(time[k]))
      vstr[vk++] = time[k];
    else {
      values[n] = -1;
      success = false;
    }
  }
  if (!success)
    Serial.printf("invalid characters in time string \"%s\"!\n", time);
  if (n == 0) {
    if (success)
      Serial.printf("invalid time string \"%s\"!\n", time);
    success = false;
  }
  int hour = values[0];
  int min = values[1];
  int sec = values[2];
  // check and set:
  if (hour < 0 || hour > 23) {
    if (success)
      Serial.printf("invalid hour %d in time string \"%s\"!\n", hour, time);
    success = false;
  }
  if (min < 0 || min > 59) {
    if (success)
      Serial.printf("Invalid minute %d in time string \"%s\"!\n", min, time);
    success = false;
  }
  if (sec < 0 || sec > 59) {
    if (success)
      Serial.printf("Invalid second %d in time string \"%s\"!\n", sec, time);
    success = false;
  }
  if (success) {
    tm.Hour = hour;
    tm.Minute = min;
    tm.Second = sec;
  }
  return success;
}


bool parseDateStr(const char *date, tmElements_t &tm) {
  // parse:
  char vstr[8] = "";
  unsigned int vk = 0;
  int values[3] = {-1, -1, -1};
  unsigned int n = 0;
  bool success = true;
  for (unsigned int k=0; k <= strlen(date); k++) {
    if (k == strlen(date) || date[k] == '-' || date[k] == 'T') {
      vstr[vk] = '\0';
      if (strlen(vstr) > 0)
	values[n++] = atoi(vstr);
      vk = 0;
      if (date[k] == 'T')
	break;
    }
    else if (isdigit(date[k]))
      vstr[vk++] = date[k];
    else
      success = false;
  }
  if (!success)
    Serial.printf("invalid characters in date string \"%s\"!\n", date);
  if (n != 3) {
    if (success)
      Serial.printf("invalid date string \"%s\"!\n", date);
    success = false;
  }
  int year = values[0];
  int month = values[1];
  int day = values[2];
  if (year >= 0 && year < 100)
    year += 2000;
  // check and set:
  if (year < 2020) {
    if (success)
      Serial.printf("invalid year %d in date string \"%s\"!\n", year, date);
    success = false;
  }
  if (month < 1 || month > 12) {
    if (success)
      Serial.printf("Invalid month %d in date string \"%s\"!\n", month, date);
    success = false;
  }
  if (day < 1 || day > 31) {
    if (success)
      Serial.printf("Invalid day %d in date string \"%s\"!\n", day, date);
    success = false;
  }
  if (success) {
    tm.Year = year - 1970;
    tm.Month = month;
    tm.Day = day;
  }
  return success;
}


bool RTClock::parseDateTimeStr(const char *datetime, tmElements_t &tm) {
  // init with current time:
  time_t t = now();
  breakTime(t, tm);
  // parse date-time string YYYY-MM-DDThh:mm:ss
  char *tp = strchr(datetime, 'T');
  if (tp == NULL) {
    if (strchr(datetime, '-') != NULL)
      return parseDateStr(datetime, tm);
    else
      return parseTimeStr(datetime, tm);
  }
  else {
    bool rd = parseDateStr(datetime, tm);
    bool rt = parseTimeStr(tp + 1, tm);
    if (rd && rt)
      return true;
  }
  return false;
}


time_t RTClock::get() {
  if (GetTime == 0)
    return now();
  return GetTime();
}


void RTClock::sync() {
  setTime(get());
}


void RTClock::set(time_t t) {
  setTime(t);
  Teensy3Clock.set(t);
}


bool RTClock::set(int year, int month, int day, int hour, int min, int sec,
		  bool from_start, bool check) {
  time_t t = checkTime(year, month, day, hour, min, sec, check);
  if (t == 0)
    return false;
  if (from_start)
    t += millis()/1000;
  set(t);
  return true;
}


bool RTClock::set(char *datetime, bool from_start) {
  tmElements_t tm;
  if (!parseDateTimeStr(datetime, tm))
    return false;
  time_t t = makeTime(tm);
  if (from_start)
    t += millis()/1000;
  set(t);
  return true;
}


bool RTClock::set(Stream &instream, Stream &outstream) {
  while (true) {
    outstream.print("Enter date and time in ISO format (YYYY-MM-DDThh:mm:dd): ");
    while (instream.available() == 0) {
      yield();
      delay(1);
    }
    char datetime[32];
    instream.readBytesUntil('\n', datetime, 32);
    outstream.println(datetime);
    if (strlen(datetime) == 0 || datetime[0] == 'q')
      return false;
    memset(datetime + strlen(datetime), '\0', 32 - strlen(datetime));
    if (set(datetime)) {
      char times[20];
      dateTime(times);
      outstream.printf("\nSuccessfully set real-time clock to: %s\n", times);
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


void RTClock::time(char *str, time_t t, bool brief,
		   bool dash, bool seconds) const {
  str[0] = '\0';
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return;
    t = now();
  }
  if (seconds) {
    if (brief)
      sprintf(str, "%02d%02d%02d", hour(t), minute(t), second(t));
    else if (dash)
      sprintf(str, "%02d-%02d-%02d", hour(t), minute(t), second(t));
    else
      sprintf(str, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  }
  else {
    if (brief)
      sprintf(str, "%02d%02d", hour(t), minute(t));
    else if (dash)
      sprintf(str, "%02d-%02d", hour(t), minute(t));
    else
      sprintf(str, "%02d:%02d", hour(t), minute(t));
  }
}


void RTClock::dateTime(char *str, time_t t, bool brief,
		       bool dash, bool seconds) const {
  str[0] = '\0';
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return;
    t = now();
  }
  if (seconds) {
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
  else {
    if (brief)
      sprintf(str, "%04d%02d%02dT%02d%02d",
	      year(t), month(t), day(t), hour(t), minute(t));
    else if (dash)
      sprintf(str, "%04d-%02d-%02dT%02d-%02d",
	      year(t), month(t), day(t), hour(t), minute(t));
    else
      sprintf(str, "%04d-%02d-%02dT%02d:%02d",
	      year(t), month(t), day(t), hour(t), minute(t));
  }
}


String RTClock::makeStr(const String &str, time_t t, bool dash) const {
  char ts[20];
  String tstr = str;
  if (t == 0) {
    if (timeStatus() == timeNotSet)
      return "";
    t = now();
  }
  if (tstr.indexOf("SDATETIMEM") >= 0) {
    dateTime(ts, t, true, dash, false);
    tstr.replace("SDATETIMEM", ts);
  }
  else if (tstr.indexOf("DATETIMEM") >= 0) {
    dateTime(ts, t, false, dash, false);
    tstr.replace("DATETIMEM", ts);
  }
  else if (tstr.indexOf("SDATETIME") >= 0) {
    dateTime(ts, t, true, dash, true);
    tstr.replace("SDATETIME", ts);
  }
  else if (tstr.indexOf("DATETIME") >= 0) {
    dateTime(ts, t, false, dash, true);
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
  else if (tstr.indexOf("STIMEM") >= 0) {
    time(ts, t, true, dash, false);
    tstr.replace("STIMEM", ts);
  }
  else if (tstr.indexOf("TIMEM") >= 0) {
    time(ts, t, false, dash, false);
    tstr.replace("TIMEM", ts);
  }
  else if (tstr.indexOf("STIME") >= 0) {
    time(ts, t, true, dash, true);
    tstr.replace("STIME", ts);
  }
  else if (tstr.indexOf("TIME") >= 0) {
    time(ts, t, false, dash, true);
    tstr.replace("TIME", ts);
  }
  return tstr;
}


void RTClock::print(Stream &stream) const {
  char times[20];
  dateTime(times);
  stream.println(times);
}


void RTClock::write(Stream &stream, size_t indent,
		     size_t indent_incr) const {
  char times[20];
  dateTime(times);
  stream.printf("%*sReal-time clock:\n", indent, "");
  indent += indent_incr;
  size_t w = 12;
  for (size_t k=0; k<NKeyVals; k++) {
    size_t kw = w >= strlen(Keys[k]) ? w - strlen(Keys[k]) : 0;
    stream.printf("%*s%s:%*s %s\n", indent, "", Keys[k], kw, "", Values[k]);
  }
  stream.printf("%*sCurrent time: %s\n", indent, "", times);
  stream.printf("%*sStatus:       ", indent, "");
  if (timeStatus() == timeSet)
    stream.println("time is set");
  else if (timeStatus() == timeNotSet)
    stream.println("time has never been set");
  else if (timeStatus() == timeNeedsSync)
    stream.println("time needs to be synchronized with RTC");
  else
    stream.println("unknown");
}

