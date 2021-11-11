#include <TestSignals.h>

Waveform wave;

float freq = 500.0;
float ampl = 0.1;
float ampls[] = {0.5, -1.0};
float phases[] = {0.0, -1.0};

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  wave.setup(140000);
  wave.setHarmonics(ampls, phases);
  wave.start(A21, freq, ampl);
}


void loop() {
}
