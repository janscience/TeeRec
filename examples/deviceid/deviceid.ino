#include <DeviceID.h>


int DIPPins[] = { 33, 34, 35, 36, -1 };
int PowerPin = 37;

DeviceID deviceid;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  Serial.println("DeviceID test");
  deviceid.set(PowerPin, DIPPins);
}


void loop() {
  deviceid.read();
  Serial.printf("%2d  %s\n", deviceid.id(), deviceid.makeStr("loggerIDA").c_str());
  delay(100);
}
