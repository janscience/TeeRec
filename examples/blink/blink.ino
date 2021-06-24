#include <Blink.h>

Blink blink;
elapsedMillis Time;


void setup() {
  blink.set(1000, 20);
}


void loop() {
  blink.update();
  if (Time > 10000) {
    Time = 0;
    uint32_t times[] = {100, 100, 100, 100, 100, 100, 0};
    blink.blink(times);
  }
}
