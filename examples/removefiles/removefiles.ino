#include <SDWriter.h>
#include <Blink.h>


#define PATH   "recordings"

SDCard sd;
Blink blink(LED_BUILTIN);


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.switchOn();
  sd.begin();
  sd.removeFiles(PATH);
  blink.switchOff();
  sd.end();
}


void loop() {
}
