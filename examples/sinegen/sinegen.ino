#include <Waveform.h>

Waveform wave;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  wave.setup(A21, 100000);
  wave.start(400.0, 0.7);
}


void loop() {
}
