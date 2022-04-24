#include <TimeLib.h>
#include <Sensors.h>
#include <TemperatureDS18x20.h>
#include <SenseBME280.h>


Sensors sensors;
TemperatureDS18x20 temp(&sensors, 10);  // DATA on pin 10
SenseBME280 bme;
TemperatureBME280 tempbme(&bme, &sensors);
HumidityBME280 hum(&bme, &sensors);
AbsoluteHumidityBME280 abshum(&bme, &sensors);
DewPointBME280 dp(&bme, &sensors);
PressureBME280 pres(&bme, &sensors);
SeaLevelPressureBME280 slpres(&bme, &sensors, 460.0);

time_t getTeensyTime() {
  return rtc_get();
}

void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setSyncProvider(getTeensyTime);  // enable real time clock
  sensors.setInterval(0.1);
  bme.beginI2C(Wire, 0x77);
  pres.setHectoPascal();
  slpres.setMilliBar();
  sensors.report();
  Serial.println();
  delay(500);
  sensors.start();
  sensors.printHeader();
}

void loop(void) {
  if (sensors.update())
    sensors.printValues();
}
