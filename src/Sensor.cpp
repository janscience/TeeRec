#include <Sensors.h>
#include <Sensor.h>


Sensor::Sensor() :
  Name(""),
  Symbol(""),
  BasicUnit(""),
  Unit(""),
  Format("%.2f"),
  Fac(1.0),
  Configured(false) {
}


Sensor::Sensor(const char *name, const char *symbol, const char *unit,
	       const char *format) :
  Sensor() {
  strcpy(BasicUnit, unit);
  setName(name);
  setSymbol(symbol);
  setUnit(unit, 1.0);
  setFormat(format);
}


Sensor::Sensor(Sensors *sensors, const char *name, const char *symbol,
	       const char *unit, const char *format) :
  Sensor(name, symbol, unit, format) {
  if (sensors != 0)
    sensors->addSensor(*this);
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


const char* Sensor::basicUnit() const {
  return BasicUnit;
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


float Sensor::value() const {
  return Fac*reading();
}


int Sensor::valueStr(char *s) const {
  return sprintf(s, Format, value());
}


void Sensor::setHectoPascal() {
  setUnit("hPa", 0.01, "%.2f");
}


void Sensor::setKiloPascal() {
  setUnit("kPa", 0.001, "%.3f");
}


void Sensor::setMegaPascal() {
  setUnit("MPa", 1e-6, "%.6f");
}


void Sensor::setBar() {
  setUnit("bar", 1e-5, "%.5f");
}


void Sensor::setMilliBar() {
  setUnit("mbar", 0.01, "%.2f");
}


void Sensor::setAt() {
  setUnit("at", 0.0000101971621298, "%.5f");
}


void Sensor::setAtm() {
  setUnit("atm", 0.00000986923266716, "%.5f");
}


void Sensor::setMMHg() {
  setUnit("mmHg", 0.00750061575846, "%.2f");
}


void Sensor::setPSI() {
  setUnit("psi", 0.00014503773773, "%.4f");
}


void Sensor::setTorr() {
  setUnit("torr", 0.00750061682704, "%.2f");
}

