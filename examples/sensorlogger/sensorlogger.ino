#include <Configurator.h>
#include <Sensors.h>
#include <Temperature.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>


// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file sensorlogger.cfg)

uint8_t tempPin = 10;         // pin for DATA line of thermometer
float sensorsInterval = 10.0; // interval between sensors readings in seconds

// ------------------------------------------------------------------------------------------

Configurator config;
RTClock rtclock;
Temperature temp;
Sensors sensors(rtclock);
SDCard sdcard;
Blink blink(LED_BUILTIN);


// ------------------------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  rtclock.report();
  sensors.addSensor(temp);
  sensors.setInterval(sensorsInterval);
  sdcard.begin();
  config.setConfigFile("sensorlogger.cfg");
  config.configure(sdcard);
  if (!temp.available() && tempPin >= 0)
    temp.begin(tempPin);
  sensors.report();
  bool success = sensors.openCSV(sdcard, "sensors");
  blink.switchOff();
  if (success) {
    sensors.start();
    blink.setSingle();
    Serial.println();
  }
  else {
    while (1) {};
  }
}


void loop() {
  if (sensors.update())
    sensors.print();
  if (sensors.pending())
    sensors.writeCSV();
  blink.update();
}
