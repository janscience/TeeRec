#include <Sensor.h>


Sensor::Sensor() :
  Name(""),
  Unit(""),
  Format("%.2f"),
  Configured(false) {
}


Sensor::Sensor(const char *name, const char *unit, const char *format) :
  Sensor() {
  setName(name);
  setUnit(unit);
  setFormat(format);
}


const char* Sensor::name() const {
  return Name;
}


void Sensor::setName(const char *name) {
  strcpy(Name, name);
}


const char* Sensor::unit() const {
  return Unit;
}


void Sensor::setUnit(const char *unit) {
  strcpy(Unit, unit);
}


const char* Sensor::format() const {
  return Format;
}


void Sensor::setFormat(const char *format) {
  strcpy(Format, format);
}


int Sensor::print(char *s) const {
  return sprintf(s, Format, value());
}


