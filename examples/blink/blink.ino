#include <Blink.h>

Blink blink;


void setup() {
  blink.setTiming(1000, 200);
}


void loop() {
  blink.update();
}
