#include <Sensors.h>
#include <TemperatureDS18x20.h>
#include <SenseBME280.h>


TemperatureDS18x20 temp(10);  // DATA on pin 10
SenseBME280 bme;
TemperatureBME280 tempbme(&bme);
HumidityBME280 hum(&bme);
AbsoluteHumidityBME280 abshum(&bme);
DewPointBME280 dp(&bme);
HeatIndexBME280 hi(&bme);
PressureBME280 pres(&bme);
SeaLevelPressureBME280 slpres(&bme, 460.0);
Sensors sensors;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  temp.setName("T");
  sensors.setInterval(0.1);
  sensors.addSensor(temp);
  bme.beginI2C(Wire, 0x77);
  sensors.addSensor(tempbme);
  sensors.addSensor(hum);
  //sensors.addSensor(abshum);
  //sensors.addSensor(dp);
  //sensors.addSensor(hi);
  //pres.setUnit("kPa", 0.001, "%.2f");
  //slpres.setUnit("kPa", 0.001, "%.2f");
  //sensors.addSensor(pres);
  //sensors.addSensor(slpres);
  sensors.start();
  sensors.printHeader();
}

void loop(void) {
  if (sensors.update())
    sensors.printValues();
}
