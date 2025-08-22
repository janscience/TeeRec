#include <Blink.h>


Blink blink("status", LED_BUILTIN);
elapsedMillis Time;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.report();
  //blink.setDouble();
  blink.setMultiple(5);
}


void loop() {
  blink.update();
  if (Time > 10000) {
    Time = 0;
    //blink.blinkTriple();
    blink.blinkSingle();
  }
}
