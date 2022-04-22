#include <Sensors.h>
#include <Temperature.h>
#include <SenseBME280.h>


Temperature temp(10);  // DATA on pin 10
SenseBME280 bme;
TemperatureBME280 tempbme(&bme);
HumidityBME280 hum(&bme);
PressureBME280 pres(&bme);
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
  pres.setUnit("kPa", 0.001, "%.2f");
  sensors.addSensor(pres);
  sensors.start();
  sensors.printHeader();
}

void loop(void) {
  if (sensors.update())
    sensors.printValues();
}
