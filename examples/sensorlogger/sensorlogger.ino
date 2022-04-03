#include <Configurator.h>
#include <Sensors.h>
#include <Temperature.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>


// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file sensorlogger.cfg)

uint8_t tempPin = 10;         // pin for DATA line of thermometer
int sensorsNFiles = 2;        // number of files used for storing sensor data
float sensorsInterval = 10.0; // interval between sensors readings in seconds

// ------------------------------------------------------------------------------------------

Configurator config;
Temperature temp;
Sensors sensors;
SDCard sdcard;
RTClock rtclock;
Blink blink;


// ------------------------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  rtclock.report();
  sensors.addSensor(temp);
  sensors.setInterval(sensorsInterval);
  sensors.setNFiles(sensorsNFiles);
  sdcard.begin();
  config.setConfigFile("sensorlogger.cfg");
  config.configure(sdcard);
  if (!temp.available() && tempPin >= 0)
    temp.begin(tempPin);
  sensors.report();
  sensors.openCSV(sdcard, "sensors", rtclock);
  sensors.start();
  blink.switchOff();
  blink.setSingle();
}


void loop() {
  if (sensors.update())
    sensors.writeCSV();
  blink.update();
}
