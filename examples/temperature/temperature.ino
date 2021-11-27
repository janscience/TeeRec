#include <Sensors.h>
#include <Temperature.h>


Temperature temp(10);  // DATA on pin 10
Sensors sensors;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  sensors.setInterval(3000);
  sensors.addSensor(&temp);
  sensors.start();
  Serial.printf("ROM = %s\n", temp.identifier());
  Serial.printf("Chip = %s\n", temp.chip());
}

void loop(void) {
  if (sensors.update())
    sensors.report();
}
