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
  return 50;  // this is generous, <10 for no oversampling , <40 for 16x oversampling
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
  setFilter(0); // 0 (off) to 4. Filter coefficient. Table 28 in data sheet.
  setStandbyTime(1); // 0 to 7 valid. Time between readings. Table 27 in data sheet.
  setTempOverSample(1); // powers of two from 0 to 16 are valid. 0 disables temp sensing. Table 24 in data sheet.
  setPressureOverSample(1); // powers of two from 0 to 16 are valid. 0 disables pressure sensing. Table 23 in data sheet.
  setHumidityOverSample(1); // powers of two from 0 to 16 are valid. 0 disables humidity sensing. table 19 in data sheet.
  setMode(MODE_SLEEP); // power down
}


SensorBME280::SensorBME280(SenseBME280 *bme, const char *name,
			   const char *symbol, const char *unit,
			   const char *format)
  : Sensor(name, symbol, unit, format),
    BME(bme) {
}


void SensorBME280::report() {
  if (available())
    Serial.printf("%s %s (%s): device %s at %.2f%s resolution.\n",
		  name(), symbol(), unit(), BME->chip(), resolution(), unit());
}


TemperatureBME280::TemperatureBME280(SenseBME280 *bme)
  : SensorBME280(bme, "temperature", "T", "ºC", "%.2f") {
}


float TemperatureBME280::resolution() const {
  return 0.01;
}


float TemperatureBME280::value() const {
  return Fac*BME->temperature();
}


HumidityBME280::HumidityBME280(SenseBME280 *bme)
  : SensorBME280(bme, "humidity", "RH", "%", "%.1f") {
}


float HumidityBME280::resolution() const {
  return 0.07;
}


float HumidityBME280::value() const {
  return Fac*BME->humidity();
}


AbsoluteHumidityBME280::AbsoluteHumidityBME280(SenseBME280 *bme)
  : SensorBME280(bme, "absolute humidity", "H", "g/m^3", "%.1f") {
}


float AbsoluteHumidityBME280::resolution() const {
  return 0.07;
}


float AbsoluteHumidityBME280::value() const {
  // from https://github.com/finitespace/BME280/blob/master/src/EnvironmentCalculations.cpp
  float temp = BME->temperature();
  float humidity = BME->humidity();
  const float mw = 18.01534; 	// molar mass of water g/mol
  const float R = 8.31447215; 	// Universal gas constant J/mol/K
  temp = pow(2.718281828, (17.67 * temp) / (temp + 243.5));
  return Fac * (6.112 * temp * humidity * mw)/((273.15 + temp) * R);
}


DewPointBME280::DewPointBME280(SenseBME280 *bme)
  : SensorBME280(bme, "dew point", "Tdp", "ºC", "%.1f") {
}


float DewPointBME280::resolution() const {
  return 0.35;
}


float DewPointBME280::value() const {
  // https://en.wikipedia.org/wiki/Dew_point
  float temp = BME->temperature();
  float humidity = BME->humidity();
  const float b = 17.62;
  const float c = 243.12;
  float gamma = log(0.01*humidity) + (b * temp)/(c + temp);
  return Fac * c * gamma/(b - gamma);
}


HeatIndexBME280::HeatIndexBME280(SenseBME280 *bme)
  : SensorBME280(bme, "heat index", "HI", "ºC", "%.0f") {
}


float HeatIndexBME280::resolution() const {
  return 1.7;
}


float HeatIndexBME280::value() const {
  // https://en.wikipedia.org/wiki/Heat_index
  double T = BME->temperature();
  double R = BME->humidity();
  const double c1 = -8.78469475556;
  const double c2 = 1.61139411;
  const double c3 = 2.33854883889;
  const double c4 = -0.14611605;
  const double c5 = -0.012308094;
  const double c6 = -0.0164248277778;
  const double c7 = 2.211732e-3;
  const double c8 = 7.2546e-4;
  const double c9 = 3.582e-6;
  float HI = c1 + c2*T + c3*R + c4*T*R + c5*T*T + c6*R*R + c7*T*T*R + c8*T*R*R + c9*T*T*R*R;
  return Fac * HI;
  // see also
  // https://github.com/finitespace/BME280/blob/master/src/EnvironmentCalculations.cpp
}


PressureBME280::PressureBME280(SenseBME280 *bme)
  : SensorBME280(bme, "pressure", "P", "Pa", "%.0f") {
}


float PressureBME280::resolution() const {
  return 3.3;
}


float PressureBME280::value() const {
  return Fac*BME->pressure();
}


SeaLevelPressureBME280::SeaLevelPressureBME280(SenseBME280 *bme, float altitude)
  : PressureBME280(bme),
    Altitude(altitude) {
  setName("sea level pressure", "Psl");
}


float SeaLevelPressureBME280::value() const {
  float pressure = BME->pressure();
  float temp = BME->temperature();
  return Fac * pressure / pow(1.0 - ((0.0065 * Altitude) / (temp + (0.0065 * Altitude) + 273.15)), 5.257);
}
