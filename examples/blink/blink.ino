#include <Blink.h>

Blink blink(LED_BUILTIN);
elapsedMillis Time;


void setup() {
  //blink.setDouble();
  blink.setMultiple(5);
}


void loop() {
  blink.update();
  if (Time > 10000) {
    Time = 0;
    blink.blinkTriple();
  }
}
