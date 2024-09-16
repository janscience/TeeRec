#include <DeviceID.h>


DeviceID::DeviceID(int id) :
  NPins(0),
  PowerPin(-1),
  PowerDelay(2),
  ID(id) {
  memset(Pins, 0, sizeof(Pins));
}


void DeviceID::set(const int *pins) {
  PowerPin = -1;
  NPins = 0;
  if (pins == 0)
    return;
  for (uint8_t k=0; k<MaxPins && pins[k]>=0; k++) {
    pinMode(pins[k], INPUT);
    Pins[NPins++] = pins[k];
  }
}


void DeviceID::set(int powerpin, const int *pins) {
  set(pins);
  PowerPin = powerpin;
  if (PowerPin >= 0 )
    pinMode(PowerPin, OUTPUT);
}


void DeviceID::set(int powerpin, int powerdelay, const int *pins) {
  set(powerpin, pins);
  PowerDelay = powerdelay;
}


int DeviceID::read() {
  int r = 0;
  int p = 1;
  if (PowerPin >= 0) {
    digitalWrite(PowerPin, 1);
    delay(PowerDelay);
  }
  for ( int k=0; k<NPins; k++) {
    if (digitalRead(Pins[k]))
      r |= p;
    p <<= 1;
  }
  if (PowerPin >= 0)
    digitalWrite(PowerPin, 0);
  ID = r;
  return ID;
}


String DeviceID::makeStr(const String &str) const {
  char ids[6];
  String istr = str;
  if (istr.indexOf("IDAA") >= 0) {
    ids[2] = '\0';
    ids[1] = char('A' + (ID % 26));
    ids[0] = char('A' + (ID / 26));
    istr.replace("IDAA", ids);
  }
  else if (istr.indexOf("IDA") >= 0) {
    ids[2] = '\0';
    ids[1] = char('A' + (ID % 26));
    ids[0] = char('A' + (ID / 26));
    if (ids[0] == 'A')
      istr.replace("IDA", ids + 1);
    else
      istr.replace("IDA", ids);
  }
  else if (istr.indexOf("ID3") >= 0) {
    sprintf(ids, "%03d", ID);
    istr.replace("ID3", ids);
  }
  else if (istr.indexOf("ID2") >= 0) {
    sprintf(ids, "%02d", ID);
    istr.replace("ID2", ids);
  }
  else if (istr.indexOf("ID") >= 0) {
    sprintf(ids, "%d", ID);
    istr.replace("ID", ids);
  }
  return istr;
}


