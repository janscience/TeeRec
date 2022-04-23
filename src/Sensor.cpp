#include <Sensor.h>


Sensor::Sensor() :
  Name(""),
  Unit(""),
  Format("%.2f"),
  Fac(1.0),
  Configured(false) {
}


Sensor::Sensor(const char *name, const char *symbol, const char *unit,
	       const char *format, float fac) :
  Sensor() {
  setName(name);
  setSymbol(symbol);
  setUnit(unit, fac);
  setFormat(format);
}


const char* Sensor::name() const {
  return Name;
}


void Sensor::setName(const char *name) {
  strcpy(Name, name);
}


void Sensor::setName(const char *name, const char *symbol) {
  strcpy(Name, name);
  strcpy(Symbol, symbol);
}


const char* Sensor::symbol() const {
  return Symbol;
}


void Sensor::setSymbol(const char *symbol) {
  strcpy(Symbol, symbol);
}


const char* Sensor::unit() const {
  return Unit;
}


void Sensor::setUnit(const char *unit, float fac) {
  strcpy(Unit, unit);
  Fac = fac;
}


void Sensor::setUnit(const char *unit, float fac, const char *format) {
  strcpy(Unit, unit);
  Fac = fac;
  strcpy(Format, format);
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

