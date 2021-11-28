#include <Blink.h>

Blink blink;
elapsedMillis Time;


void setup() {
  blink.setDouble();
}


void loop() {
  blink.update();
  if (Time > 10000) {
    Time = 0;
    blink.blinkTriple();
  }
}
