#include <TestSignals.h>
#include <Blink.h>
#include <PushButtons.h>

// ----- settings: -----------------------------------------------------------

float freq = 500.0;
float ampl = 0.5;
float rate = 100000.0;

// ----- pins: ---------------------------------------------------------------

#define FREQ_UP_PIN     1
#define FREQ_DOWN_PIN   2
#define AMPL_UP_PIN     3
#define AMPL_DOWN_PIN   4

// ---------------------------------------------------------------------------

Waveform wave;
Blink blink("status", LED_BUILTIN);
PushButtons buttons;


void frequency_up(int id) {
  rate *= pow(2.0, 1.0/12);
  wave.restart(rate);
}


void frequency_down(int id) {
  rate /= pow(2.0, 1.0/12);
  wave.restart(rate);
}


void amplitude_up(int id) {
  ampl *= 1.1;
  if (ampl > 1.0) 
    ampl = 1.0;
  wave.setAmplitude(ampl);
}


void amplitude_down(int id) {
  ampl /= 1.1;
  if (ampl < 0.0001) 
    ampl = 0.0001;
  wave.setAmplitude(ampl);
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 100) {};
  blink.setSingle();
  buttons.add(FREQ_UP_PIN, INPUT_PULLUP, frequency_up);
  buttons.add(FREQ_DOWN_PIN, INPUT_PULLUP, frequency_down);
  buttons.add(AMPL_UP_PIN, INPUT_PULLUP, amplitude_up);
  buttons.add(AMPL_DOWN_PIN, INPUT_PULLUP, amplitude_down);
  wave.setup(rate);
  wave.start(A21, freq, 1.0);
  wave.setAmplitude(ampl);
}


void loop() {
  buttons.update();
  blink.update();
}
