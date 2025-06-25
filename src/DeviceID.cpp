#include <DeviceID.h>


DeviceID::DeviceID(int id, int powerdelay) :
  ID(id),
  Source(0),
  NPins(0),
  PowerPin(-1),
  PowerDelay(powerdelay) {
  memset(Pins, 0, sizeof(Pins));
  if (id > 0)
    Source = 1;
}


void DeviceID::setID(int id) {
  ID = id;
  Source = 2;
}


String DeviceID::makeStr(const String &str) const {
  char ids[16];
  String istr = str;
  int ida = ID - 1;
  if (istr.indexOf("IDAA") >= 0) {
    ids[2] = '\0';
    ids[1] = char('A' + (ida % 26));
    ids[0] = char('A' + (ida / 26));
    istr.replace("IDAA", ids);
  }
  else if (istr.indexOf("IDA") >= 0) {
    ids[2] = '\0';
    ids[1] = char('A' + (ida % 26));
    ids[0] = char('A' + (ida / 26));
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


void DeviceID::report(Stream &stream) const {
  char ss[20];
  switch (Source) {
  case 1:
    strcpy(ss, "default");
    break;
  case 2:
    strcpy(ss, "configured");
    break;
  case 3:
    strcpy(ss, "read from device");
    break;
  default:
    strcpy(ss, "not set");
  }
  stream.printf("Device identifier (%s): %d\n\n", ss, ID);
}


void DeviceID::setPins(const int *pins) {
  PowerPin = -1;
  NPins = 0;
  if (pins == 0)
    return;
  for (uint8_t k=0; k<MaxPins && pins[k]>=0; k++) {
    pinMode(pins[k], INPUT);
    Pins[NPins++] = pins[k];
  }
}


void DeviceID::setPins(int powerpin, const int *pins) {
  setPins(pins);
  PowerPin = powerpin;
  if (PowerPin >= 0 )
    pinMode(PowerPin, OUTPUT);
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
  if (r == 0)
    return -1;
  ID = r;
  Source = 3;
  return ID;
}

