#include <SDWriter.h>
#include <Blink.h>


char path[] = "recordings";

SDWriter file;
Blink blink;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.switchOn();
  file.removeFiles(path);
  blink.switchOff();
}


void loop() {
}
