#include <Sensors.h>


Sensors::Sensors() {
  NSensors = 0;
  MaxDelay = 0;
  Interval = 10000;
  Time = 0;
  State = 0;
}


void Sensors::addSensor(Sensor *sensor) {
  if (NSensors >= MaxSensors) {
    Serial.println("Maximum number of supported sensors exceeded!");
    return;
  }
  if (sensor->available()) {
    Snsrs[NSensors++] = sensor;
    if (sensor->delay() > MaxDelay)
      MaxDelay = sensor->delay();
  }
}


void Sensors::setInterval(unsigned long interval) {
  Interval = interval;
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


void Sensors::report() {
  for (uint8_t k=0; k<NSensors; k++)
    Serial.printf("%s = %5.2f%s\n", Snsrs[k]->parameter(),
		  Snsrs[k]->value(), Snsrs[k]->unit());
}
