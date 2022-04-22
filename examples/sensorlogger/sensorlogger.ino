#include <Configurator.h>
#include <Sensors.h>
#include <Temperature.h>
#include <SenseBME280.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>


// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file sensorlogger.cfg)

uint8_t tempPin = 10;         // pin for DATA line of thermometer
float sensorsInterval = 2.0; // interval between sensors readings in seconds

// ------------------------------------------------------------------------------------------

Configurator config;
RTClock rtclock;
Temperature temp;
SenseBME280 bme;
TemperatureBME280 tempbme(&bme);
HumidityBME280 hum(&bme);
PressureBME280 pres(&bme);
Sensors sensors(rtclock);
SDCard sdcard;
Blink blink(LED_BUILTIN);
bool symbols = false;


// ------------------------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  rtclock.report();
  sensors.addSensor(temp);
  bme.beginI2C(Wire, 0x77);
  sensors.addSensor(tempbme);
  sensors.addSensor(hum);
  sensors.addSensor(pres);
  sensors.setInterval(sensorsInterval);
  sdcard.begin();
  config.setConfigFile("sensorlogger.cfg");
  config.configure(sdcard);
  if (!temp.available() && tempPin >= 0)
    temp.begin(tempPin);
  sensors.report();
  bool success = sensors.openCSV(sdcard, "sensors", symbols);
  blink.switchOff();
  if (success) {
    sensors.start();
    blink.setSingle();
    Serial.println();
  }
  else {
    Serial.println();
    Serial.println("ERROR: SD card no available -> halt!");
    while (1) {};
  }
}


void loop() {
  if (sensors.update()) {
    sensors.print(symbols);
    Serial.println();
  }
  if (sensors.pending())
    sensors.writeCSV();
  blink.update();
}
