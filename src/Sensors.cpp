#include <Sensors.h>


Sensors::Sensors() :
  Configurable("Sensors") {
  NSensors = 0;
  MaxDelay = 0;
  Interval = 10000;
  UseInterval = Interval;
  Time = 0;
  State = 0;
  RTC = 0;
  NFiles = 1;
  CFile = 0;
}


void Sensors::addSensor(Sensor &sensor) {
  if (NSensors >= MaxSensors) {
    Serial.println("Maximum number of supported sensors exceeded!");
    return;
  }
  Snsrs[NSensors++] = &sensor;
}


void Sensors::setNFiles(int nfiles) {
  NFiles = nfiles;
  if (NFiles > MaxFiles)
    NFiles = MaxFiles;
}


void Sensors::setInterval(float interval) {
  Interval = (unsigned long)(1000.0*interval);
}


void Sensors::report() {
  char ds[2] = {'\0', '\0'};
  if (NSensors > 1)
    ds[1] = 's';
  Serial.printf("Sensors: %d device%s, read every %gs, stored in %d files\n",
		NSensors, ds, 0.001*Interval, NFiles);
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
  if (NFiles > 0)
    UseInterval = Interval / NFiles;
  if (UseInterval < 2*MaxDelay)
    UseInterval = 2*MaxDelay;
  Time = UseInterval - MaxDelay;
  State = 0;
  CFile = NFiles-1;
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
      State = 0;
      Time -= UseInterval;
      return true;
    }
    break;
  }
  return false;
}


void Sensors::print() {
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      Serial.printf("%s = %5.2f%s\n", Snsrs[k]->name(),
		    Snsrs[k]->value(), Snsrs[k]->unit());
    else
      Serial.printf("%s not available\n", Snsrs[k]->name());
  }
}


bool Sensors::openCSV(SDCard &sd, const char *path,
		      RTClock &rtc, bool append) {
  CFile = NFiles - 1;
  RTC = &rtc;
  // compose header line:
  size_t n = 5;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      n += strlen(Snsrs[k]->name()) + strlen(Snsrs[k]->unit()) + 2;
  }
  char s[n];
  char *sp = s;
  sp += sprintf(sp, "time,");
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      sp += sprintf(sp, "%s/%s,", Snsrs[k]->name(), Snsrs[k]->unit());
  }
  *(--sp) = '\n';
  // create files and write header:
  char fpath[strlen(path)+10];
  char ns[2];
  strcpy(ns, "1");
  bool success = true;
  for (int nfiles=0; nfiles < NFiles; nfiles++) {
    strcpy(fpath, path);
    if (NFiles > 1) {
      strcat(fpath, ns);
      ns[0]++;
    }
    strcat(fpath, ".csv");
    if (append && sd.exists(fpath))
      DF[nfiles] = sd.openAppend(fpath);
    else
      DF[nfiles] = sd.openWrite(fpath);
    if (DF[nfiles]) {
      DF[nfiles].write(s, strlen(s));
      DF[nfiles].flush();
    }
    else
      success = false;
  }
  return success;
}


bool Sensors::writeCSV() {
  // next file:
  CFile++;
  if (CFile >= NFiles)
    CFile = 0;
  if (CFile >= NFiles || !DF[CFile])
    return false;
  // get time:
  char ts[20];
  if (RTC != 0)
    RTC->dateTime(ts);
  else
    ts[0] = '\0';
  // compose data line:
  size_t n = 20 + NSensors*10;
  char s[n];
  char *sp = s;
  sp += sprintf(sp, "%s,", ts);
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      sp += Snsrs[k]->print(sp);
      *(++sp) = ',';
    }
  }
  *(--sp) = '\n';
  *(++sp) = '\0';
  // write data:
  DF[CFile].write(s, strlen(s));
  DF[CFile].flush();
  return bool(DF[CFile]);
}


void Sensors::configure(const char *key, const char *val) {
  bool found = true;
  char pval[30];
  if (strcmp(key, "writeinterval") == 0) {
    setInterval(parseTime(val));
    sprintf(pval, "%gs", 0.001*Interval);
    Serial.printf("  set Sensors-%s to %s\n", key, pval);
  }
  else if (strcmp(key, "nfiles") == 0) {
    setNFiles(atoi(val));
    sprintf(pval, "%d", NFiles);
    Serial.printf("  set Sensors-%s to %s\n", key, pval);
  }
  else {
    found = false;
    for (uint8_t k=0; k<NSensors; k++) {
      if (Snsrs[k]->configure(key, val)) {
	found = true;
	break;
      }
    }
  }
  if (!found)
    Serial.printf("  Sensors key \"%s\" not found.\n", key);
}
