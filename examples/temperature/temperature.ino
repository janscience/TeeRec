#include <Sensors.h>
#include <Temperature.h>
#include <SDWriter.h>
#include <RTClock.h>


Temperature temp(10);  // DATA on pin 10
Sensors sensors;
SDCard sdcard;
RTClock rtc;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  sdcard.begin();
  sensors.setInterval(3.0);
  sensors.addSensor(temp);
  sensors.writeCSVHeader(sdcard, "temperatures.csv", rtc);
  sensors.start();
  Serial.printf("ROM = %s\n", temp.identifier());
  Serial.printf("Chip = %s\n", temp.chip());
}

void loop(void) {
  if (sensors.update()) {
    sensors.report();
    sensors.writeCSV();
  }
}
