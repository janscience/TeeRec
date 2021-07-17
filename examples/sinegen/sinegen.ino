#include <TestSignals.h>

Waveform wave;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  wave.setup(140000);
  wave.start(A21, 500.0, 0.5);
}


void loop() {
}
