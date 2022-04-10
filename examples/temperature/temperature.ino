#include <Sensors.h>
#include <Temperature.h>


Temperature temp(10);  // DATA on pin 10
Sensors sensors;


void setup(void) {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  temp.setName("T");
  sensors.setInterval(1.0);
  sensors.addSensor(temp);
  sensors.start();
  sensors.printHeader();
}

void loop(void) {
  if (sensors.update())
    sensors.printValues();
}
