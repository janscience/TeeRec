#include <DeviceID.h>


DeviceID::DeviceID(int id, int powerdelay) :
  ID(id),
  Source(0),
  NPins(0),
  Pullup(true),
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


String DeviceID::makeStr(const String &str, bool nozero) const {
  int id = ID;
  if (id == 0 && nozero)
    id = 1 << NPins;
  int ida = id - 1;
  char ids[16];
  String istr = str;
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
    sprintf(ids, "%03d", id);
    istr.replace("ID3", ids);
  }
  else if (istr.indexOf("ID2") >= 0) {
    sprintf(ids, "%02d", id);
    istr.replace("ID2", ids);
  }
  else if (istr.indexOf("ID") >= 0) {
    sprintf(ids, "%d", id);
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
  stream.printf("Device identifier (%s): %d (#%02X)\n\n", ss, ID, ID);
}


void DeviceID::setPins(const int *pins, bool pullup) {
  Pullup = pullup;
  PowerPin = -1;
  NPins = 0;
  if (pins == 0)
    return;
  for (uint8_t k=0; k<MaxPins && pins[k]>=0; k++) {
    pinMode(pins[k], Pullup ? INPUT_PULLUP : INPUT);
    Pins[NPins++] = pins[k];
  }
}


void DeviceID::setPins(int powerpin, const int *pins, bool pullup) {
  setPins(pins, pullup);
  PowerPin = powerpin;
  if (PowerPin >= 0 )
    pinMode(PowerPin, OUTPUT);
}


int DeviceID::read(Stream *stream) {
  if (NPins == 0)
    return -1;
  if (PowerPin >= 0) {
    digitalWrite(PowerPin, 1);
    delay(PowerDelay);
  }
  int r = 0;
  int p = 1;
  for (int k=0; k<NPins; k++) {
    int d = digitalRead(Pins[k]);
    if (Pullup) {
      if (!d)
	r |= p;
      if (stream != 0)
	stream->printf("Read DeviceID pin %d (value %d, inverted): %d\n",
		       k, p, !d);
    }
    else {
      if (d)
	r |= p;
      if (stream != 0)
	stream->printf("Read DeviceID pin %d (value %d): %d\n",
		       k, p, d);
    }
    p <<= 1;
  }
  if (PowerPin >= 0)
    digitalWrite(PowerPin, 0);
  if (stream != 0)
    stream->printf("Read DeviceID: #%02X = %02d\n", r, r);
  ID = r;
  Source = 3;
  return ID;
}

