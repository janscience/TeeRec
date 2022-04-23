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


const char* Sensor::chip() const {
  return "";
}


const char* Sensor::identifier() const {
  return "";
}


void Sensor::report() {
  if (available()) {
    char rs[10];
    resolutionStr(rs);
    Serial.printf("%s %s (%s):", name(), symbol(), unit());
    if (strlen(chip()) > 0)
      Serial.printf("\t on %s device", chip());
    if (strlen(identifier()) > 0)
      Serial.printf(" (ID: %s)", identifier());
    Serial.printf(" at %s%s resolution.\n", rs, unit());
  }
}


int Sensor::resolutionStr(char *s) const {
  return sprintf(s, Format, resolution());
}


int Sensor::valueStr(char *s) const {
  return sprintf(s, Format, value());
}


