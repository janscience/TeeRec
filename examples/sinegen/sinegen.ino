#include <TestSignals.h>
#include <Blink.h>

Waveform wave;
Blink blink(LED_BUILTIN);

float freq = 500.0;
float ampl = 0.2;
float ampls[] = {0.4, -1.0};
float phases[] = {0.0, -1.0};

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.setSingle();
  wave.setup(140000);
  wave.setHarmonics(ampls, phases);
  wave.start(A21, freq, ampl);
}


void loop() {
  blink.update();
}
