#include <SDWriter.h>
#include <Blink.h>


char path[] = "recordings";

SDCard sd;
Blink blink;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.switchOn();
  sd.begin();
  sd.removeFiles(path);
  blink.switchOff();
  sd.end();
}


void loop() {
}
