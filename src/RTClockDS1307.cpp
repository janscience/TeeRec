#include <DS1307RTC.h>
#include <RTClockDS1307.h>


RTClockDS1307::RTClockDS1307() :
  RTClock() {
}


bool RTClockDS1307::begin() {
  tmElements_t tm;
  if (RTC.chipPresent() && RTC.read(tm)) {
    setI2CBus(Wire, 0x68);
    setChip("DS1307/DS1337/DS3231");
    setSyncProvider(RTC.get);
    return true;
  }
  return false;
}


void RTClockDS1307::set(time_t t) {
  RTC.set(t);
  RTClock::set(t);
}

