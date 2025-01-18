#include <DS1307RTC.h>
#include <RTClockDS1307.h>


#define DS1307_CTRL_ID 0x68


RTClockDS1307::RTClockDS1307() :
  RTClock() {
}


bool RTClockDS1307::begin() {
  tmElements_t tm;
  if (RTC.read(tm) && RTC.chipPresent()) {
    setI2CBus(Wire, DS1307_CTRL_ID);
    setChip("DS1307/DS1337/DS3231");
    // only DS3231 has temperature at 0x11:
    Wire.beginTransmission(DS1307_CTRL_ID);
    Wire.write((uint8_t)0x11); 
    if (Wire.endTransmission() == 0) {
      Wire.requestFrom(DS1307_CTRL_ID, 2);
      if (Wire.available() == 2) {
	int8_t temp1 = Wire.read(); 
	Wire.read();
	if (temp1 != 0) // let's assume a temperature != 0 ...
	  setChip("DS3231");
	else
	  setChip("DS1307/DS1337");
      }
    }
    setSyncProvider(RTC.get);
    return true;
  }
  return false;
}


void RTClockDS1307::set(time_t t) {
  RTC.set(t);
  RTClock::set(t);
}

