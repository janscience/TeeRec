#include <Sensors.h>
#include <Temperature.h>
#include <SDWriter.h>
#include <RTClock.h>


Temperature temp(10);  // DATA on pin 10
Sensors sensors;
SDCard sdcard;
RTClock rtclock;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  sdcard.begin();
  sensors.setInterval(3.0);
  sensors.addSensor(temp);
  sensors.report();
  sensors.writeCSVHeader(sdcard, "temperatures.csv", rtclock);
  sensors.start();
  Serial.println();
}

void loop(void) {
  if (sensors.update()) {
    sensors.print();
    sensors.writeCSV();
  }
}
