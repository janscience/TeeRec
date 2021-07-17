#include <Waveform.h>

Waveform wave;


void setup() {
  wave.setup(A21, 100000);
  wave.start(400.0, 0.7);
}


void loop() {
}
