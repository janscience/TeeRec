#include <SenseBME280.h>


SenseBME280::SenseBME280() :
  BME280() {
  memset(Chip, 0, sizeof(Chip));
  Measuring = false;
  Celsius = NoValue;
  Humidity = NoValue;
  Pressure = NoValue;
}

  
bool SenseBME280::beginI2C(uint8_t address) {
  settings.I2CAddress = address;
  if (! BME280::beginI2C())
    return false;
  init();
  return true;
}


bool SenseBME280::beginI2C(TwoWire &wire, uint8_t address) {
  settings.I2CAddress = address;
  if (! BME280::beginI2C(wire))
    return false;
  init();
  return true;
}


bool SenseBME280::beginSPI(uint8_t cs_pin) {
  if (! BME280::beginSPI(cs_pin))
    return false;
  init();
  return true;
}


bool SenseBME280::available() {
  return (Chip[0] != '\0');
}


void SenseBME280::request() {
  if (Measuring)
    return;
  setMode(MODE_FORCED); // wake up sensor and take reading
  Measuring = true;
}


unsigned long SenseBME280::delay() const
{
  return 1000;
}


void SenseBME280::read() {
  BME280_SensorMeasurements measurements;
  if (!Measuring)
    return;
  readAllMeasurements(&measurements, 0);
  Celsius = measurements.temperature;
  Humidity = measurements.humidity;
  Pressure = measurements.pressure;
  Measuring = false;
}


void SenseBME280::init() {
  memset(Chip, 0, sizeof(Chip));
  uint8_t chip_id = readRegister(BME280_CHIP_ID_REG);
  switch (chip_id) {
  case 0x58: strcpy(Chip, "BMP280"); break;
  case 0x60: strcpy(Chip, "BME280"); break;
  }
  // XXX Configure timing, oversampling, and filtering!
  /*
  setFilter(1); //0 to 4 is valid. Filter coefficient. See 3.4.4
  setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.

  setTempOverSample(1); //0 to 16 are valid. 0 disables temp sensing. See table 24.
  setPressureOverSample(1); //0 to 16 are valid. 0 disables pressure sensing. See table 23.
  setHumidityOverSample(1); //0 to 16 are valid. 0 disables humidity sensing. See table 19.
  */
  setMode(MODE_SLEEP); // power down
}


TemperatureBME280::TemperatureBME280(SenseBME280 *bme)
  : Sensor("temp", "ºC"),
    BME(bme) {
}


float TemperatureBME280::resolution() const {
  return 0.01;
}


void TemperatureBME280::report() {
  if (available())
    Serial.printf("%s (%s): temperature device %s at %.2f%s resolution.\n",
		  name(), unit(), BME->chip(), resolution(), unit());
}


int TemperatureBME280::print(char *s) const {
  return sprintf(s, "%.2f", value());
}


HumidityBME280::HumidityBME280(SenseBME280 *bme)
  : Sensor("humidity", "%"),
    BME(bme) {
}


float HumidityBME280::resolution() const {
  return 0.07;
}


void HumidityBME280::report() {
  if (available())
    Serial.printf("%s (%s): humidity device %s at %.2f%s resolution.\n",
		  name(), unit(), BME->chip(), resolution(), unit());
}


int HumidityBME280::print(char *s) const {
  return sprintf(s, "%.1f", value());
}


PressureBME280::PressureBME280(SenseBME280 *bme)
  : Sensor("pressure", "Pa"),
    BME(bme) {
}


float PressureBME280::resolution() const {
  return 3.3;
}


void PressureBME280::report() {
  if (available())
    Serial.printf("%s (%s): pressure device %s at %.2f%s resolution.\n",
		  name(), unit(), BME->chip(), resolution(), unit());
}


int PressureBME280::print(char *s) const {
  return sprintf(s, "%.0f", value());
}

