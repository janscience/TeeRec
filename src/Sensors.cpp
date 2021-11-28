#include <Sensors.h>


Sensors::Sensors() {
  NSensors = 0;
  MaxDelay = 0;
  Interval = 10000;
  Time = 0;
  State = 0;
  RTC = 0;
}


void Sensors::addSensor(Sensor &sensor) {
  if (NSensors >= MaxSensors) {
    Serial.println("Maximum number of supported sensors exceeded!");
    return;
  }
  if (sensor.available()) {
    Snsrs[NSensors++] = &sensor;
    if (sensor.delay() > MaxDelay)
      MaxDelay = sensor.delay();
  }
}


void Sensors::setInterval(float interval) {
  Interval = (unsigned long)(1000.0*interval);
}


void Sensors::report() {
  for (uint8_t k=0; k<NSensors; k++)
    Snsrs[k]->report();
}


void Sensors::start() {
  Time = Interval - MaxDelay;
  State = 0;
}


bool Sensors::update() {
  switch (State) {
  case 0: if (Time > Interval - MaxDelay) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->request();
      State = 1;
    }
    break;
  case 1: if (Time > Interval) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->read();
      State = 0;
      Time -= Interval;
      return true;
    }
    break;
  }
  return false;
}


void Sensors::print() {
  for (uint8_t k=0; k<NSensors; k++)
    Serial.printf("%s = %5.2f%s\n", Snsrs[k]->name(),
		  Snsrs[k]->value(), Snsrs[k]->unit());
}


bool Sensors::writeCSVHeader(SDCard &sd, const char *path,
			     RTClock &rtc, bool append) {
  // compose header line:
  size_t n = 5;
  for (uint8_t k=0; k<NSensors; k++)
    n += strlen(Snsrs[k]->name()) + strlen(Snsrs[k]->unit()) + 2;
  char s[n];
  char *sp = s;
  sp += sprintf(sp, "time,");
  for (uint8_t k=0; k<NSensors; k++)
    sp += sprintf(sp, "%s/%s,", Snsrs[k]->name(), Snsrs[k]->unit());
  *(--sp) = '\n';
  // create file and write header:
  RTC = &rtc;
  if (append && sd.exists(path)) {
    DF = sd.openAppend(path);
    return true;
  }
  DF = sd.openWrite(path);
  if (DF) {
    DF.write(s, strlen(s));
    DF.close();
    return true;
  }
  else
    return false;
}


bool Sensors::writeCSV() {
  if (!DF)
    return false;
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
    sp += Snsrs[k]->print(sp);
    *(++sp) = ',';
  }
  *(--sp) = '\n';
  *(++sp) = '\0';
  // write data:
  DF.write(s, strlen(s));
  DF.flush();
  return bool(DF);
}
