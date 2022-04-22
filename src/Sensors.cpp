#include <Sensors.h>


Sensors::Sensors() :
  Configurable("Sensors"),
  NSensors(0),
  MaxDelay(0),
  Interval(10000),
  Time(0),
  State(0),
  DF(),
  MData(0),
  RTC(0) {
  UseInterval = Interval;
  Header[0] = '\0';
  Data[0] = '\0';
}


Sensors::Sensors(RTClock &rtc) :
  Sensors() {
  RTC = &rtc;
}


void Sensors::addSensor(Sensor &sensor) {
  if (NSensors >= MaxSensors) {
    Serial.println("Maximum number of supported sensors exceeded!");
    return;
  }
  Snsrs[NSensors++] = &sensor;
}


float Sensors::interval() const {
  return 0.001*Interval;
}


void Sensors::setInterval(float interval) {
  Interval = (unsigned long)(1000.0*interval);
}


void Sensors::report() {
  char ds[2] = {'\0', '\0'};
  if (NSensors > 1)
    ds[1] = 's';
  Serial.printf("Sensors: %d device%s, read every %gs\n",
		NSensors, ds, 0.001*Interval);
  int n = 0;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      Serial.printf("  ");
      Snsrs[k]->report();
      n++;
    }
  }
  if (n == 0)
    Serial.println("  no sensors avilable!");
}


void Sensors::start() {
  MaxDelay = 0;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available() && Snsrs[k]->delay() > MaxDelay)
      MaxDelay = Snsrs[k]->delay();
  }
  UseInterval = Interval;
  if (UseInterval < 2*MaxDelay)
    UseInterval = 2*MaxDelay;
  Time = UseInterval - MaxDelay;
  State = 0;
  Data[0] = '\0';
}


bool Sensors::update() {
  switch (State) {
  case 0: if (Time > UseInterval - MaxDelay) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->request();
      State = 1;
    }
    break;
  case 1: if (Time > UseInterval) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->read();
      makeCSVData();
      State = 0;
      Time -= UseInterval;
      return true;
    }
    break;
  }
  return false;
}


bool Sensors::pending() {
  if (DF)
    return (strlen(Data) > NData/2 && !DF.isBusy());
  return false;
}


void Sensors::print() {
  char s[20];
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      Snsrs[k]->print(s);
      Serial.printf("%s = %s%s\n", Snsrs[k]->name(), s, Snsrs[k]->unit());
    }
    else
      Serial.printf("%s not available\n", Snsrs[k]->name());
  }
}


void Sensors::printHeader() {
  int n = 0;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      if (n > 0)
	Serial.print('\t');
      Serial.print(Snsrs[k]->name());
      if (strlen(Snsrs[k]->unit()) > 0)
	Serial.printf("/%s\t", Snsrs[k]->unit());
      n++;
    }
  }
  Serial.println();
}


void Sensors::printValues() {
  int n = 0;
  char s[20];
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      if (n > 0)
	Serial.print('\t');
      Snsrs[k]->print(s);
      Serial.print(s);
      n++;
    }
  }
  Serial.println();
}


void Sensors::setRTClock(RTClock &rtc) {
  RTC = &rtc;
}


bool Sensors::makeCSVHeader() {
  Header[0] = '\0';
  Data[0] = '\0';
  MData = 0;
  // size of header and data line:
  size_t m = 0;
  size_t n = 6;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      n += strlen(Snsrs[k]->name()) + strlen(Snsrs[k]->unit()) + 2;
      m++;
    }
  }
  if (m == 0) // no sensors
    return false;
  if (n > NHeader) // header too long
    return false;
  MData = 20 + m*10;
  // compose header line:
  char *hp = Header;
  hp += sprintf(hp, "time,");
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      if (strlen(Snsrs[k]->unit()) == 0)
	hp += sprintf(hp, "%s,", Snsrs[k]->name());
      else
	hp += sprintf(hp, "%s/%s,", Snsrs[k]->name(), Snsrs[k]->unit());
    }
  }
  *(--hp) = '\n';
  return true;
}


bool Sensors::makeCSVData() {
  if (MData == 0)
    return false;
  if (strlen(Data) > NData - MData) {
    Serial.println("WARNING: overflow of sensors data!");
    return false;
  }
  // get time:
  char ts[20];
  if (RTC != 0)
    RTC->dateTime(ts);
  else
    ts[0] = '\0';
  // compose data line:
  char *sp = Data + strlen(Data);
  sp += sprintf(sp, "%s,", ts);
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      sp += Snsrs[k]->print(sp);
      *(sp++) = ',';
    }
  }
  *(--sp) = '\n';
  *(++sp) = '\0';
  return true;
}


bool Sensors::openCSV(SDCard &sd, const char *path, bool append) {
  if (DF)
    closeCSV();
  if (Header[0] == '\0')
    makeCSVHeader();
  if (Header[0] == '\0') // no sensors
    return false;
  // create file and write header:
  char fpath[strlen(path)+10];
  strcpy(fpath, path);
  strcat(fpath, ".csv");
  if (append && sd.exists(fpath))
    DF = sd.openAppend(fpath);
  else
    DF = sd.openWrite(fpath);
  if (DF) {
    DF.write(Header, strlen(Header));
    DF.flush();
    return true;
  }
  else
    return false;
}


bool Sensors::writeCSV() {
  if (Data[0] == '\0' || !DF)
    return false;
  // write data:
  bool success = true;
  size_t n = DF.write(Data, strlen(Data));
  if (n < strlen(Data))
    success = false;
  DF.flush();
  Data[0] = '\0';        // clear buffer
  if (!DF)
    success = false;
  return success;
}


bool Sensors::closeCSV() {
  if (!DF)
    return false;
  writeCSV();
  Data[0] = '\0';        // clear buffer
  return (!DF.close());
}


void Sensors::configure(const char *key, const char *val) {
  bool found = true;
  char pval[30];
  if (strcmp(key, "writeinterval") == 0) {
    setInterval(parseTime(val));
    sprintf(pval, "%gs", 0.001*Interval);
    Serial.printf("  set Sensors-%s to %s\n", key, pval);
  }
  else {
    found = false;
    for (uint8_t k=0; k<NSensors; k++) {
      if (Snsrs[k]->configure(key, val)) {
	Snsrs[k]->setConfigured();
	found = true;
	break;
      }
    }
  }
  if (!found)
    Serial.printf("  Sensors key \"%s\" not found.\n", key);
}
