#include <Blink.h>
#include <PCA9536DigitalIO.h>

PCA9536DigitalIO gpio;

Blink blink("status", LED_BUILTIN);
elapsedMillis Time;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  Wire2.begin();
  gpio.begin(Wire2);
  blink.setPin(gpio, 0);
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
