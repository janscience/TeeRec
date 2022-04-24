#include <Sensors.h>
#include <Sensor.h>


Sensor::Sensor() :
  Name(""),
  Symbol(""),
  BasicUnit(""),
  Unit(""),
  Format("%.2f"),
  Factor(1.0),
  Offset(0.0),
  Measuring(false),
  TimeStamp(0) {
}


Sensor::Sensor(const char *name, const char *symbol, const char *unit,
	       const char *format) :
  Sensor() {
  strcpy(BasicUnit, unit);
  setName(name);
  setSymbol(symbol);
  setUnit(unit, 1.0, 0.0);
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


void Sensor::setUnit(const char *unit, float factor, float offset) {
  strcpy(Unit, unit);
  Factor = factor;
  Offset = offset;
}


void Sensor::setUnit(const char *unit, float factor, float offset,
		     const char *format) {
  strcpy(Unit, unit);
  Factor = factor;
  Offset = offset;
  strcpy(Format, format);
}


void Sensor::setUnit(const char *unit, float factor, const char *format) {
  strcpy(Unit, unit);
  Factor = factor;
  Offset = 0.0;
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


int Sensor::resolutionStr(char *s) const {
  return sprintf(s, Format, resolution());
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


void Sensor::request() {
  if (Measuring)
    return;
  requestData();
  Measuring = true;
}


void Sensor::requestData() {
}


void Sensor::read() {
  if (!Measuring)
    return;
  readData();
  TimeStamp = now();
  Measuring = false;
}


time_t Sensor::timeStamp() const {
  return TimeStamp;
}


float Sensor::value() const {
  return Factor*reading() + Offset;
}


int Sensor::valueStr(char *s) const {
  return sprintf(s, Format, value());
}


void Sensor::setKelvin() {
  setUnit("K", 1.0, 273.15, "%.2f");
}


void Sensor::setFahrenheit() {
  setUnit("F", 9.0/5.0, 32.0, "%.2f");
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

