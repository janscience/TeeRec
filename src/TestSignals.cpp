#include <Arduino.h>
#include <TeensyBoard.h>
#include <TestSignals.h>


void testSignal(int pin, int frequency, float dc) {
  pinMode(pin, OUTPUT);
  analogWriteFrequency(pin, frequency);
  analogWrite(pin, int(dc*256));
}


void setupTestSignals(int *pins, int frequency) {
  Serial.println("Test signals:");
  // pins 0-10:
#if defined(TEENSY40) || defined(TEENSY41)
  int pintimer[MaxSignalPins] = {1, 2, 3, 3, 4, 5, 6, 7, 7, 6, 8};
#elif defined(TEENSY35) || defined(TEENSY36)
  int pintimer[MaxSignalPins] = {0, 0, 1, 2, 2, 3, 3, 1, 1, 3, 3};
#elif defined(TEENSY32)
  int pintimer[MaxSignalPins] = {0, 0, 0, 1, 1, 2, 2, 0, 0, 2, 2};
#else
  #error "Board not supported for PWM signal generation."
#endif
  // setup frequencies and duty cycles:
  const int maxtimers = 9;
  int timerfreqs[maxtimers] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
  int pintimercount[maxtimers] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int maxdcs = 4;
  float pintimerdc[maxdcs] = {0.5, 0.2, 0.8, 0.33};  // duty cycle
  float maxfreqfac = 0.5;
  int freqs[MaxSignalPins];
  float dcs[MaxSignalPins];
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    int pin = pins[k];
    int timerid = pintimer[pin];
    if (timerid > 0) {
      if (timerfreqs[timerid] < 0) {
	maxfreqfac *= 2.0;
	timerfreqs[timerid] = maxfreqfac*frequency;
      }
      freqs[k] = timerfreqs[timerid];
      dcs[k] = pintimerdc[pintimercount[timerid]%maxdcs];
      pintimercount[timerid]++;
    }
    else {
      freqs[k] = -1;
      dcs[k] = -1.0;
    }
  }
  // activate pwm pins:
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    if (freqs[k] > 0)
      testSignal(pins[k], freqs[k], dcs[k]);
  }
  // report:
  for (int k=0; k<MaxSignalPins && pins[k]>=0; k++) {
    if (freqs[k] > 0)
      Serial.printf("  pin %2d: %5dHz, %3d%% duty cycle\n", pins[k], freqs[k], int(100.0*dcs[k]));
  }
  Serial.println();
}


void setupTestSignals(int pin0, int pin1, int frequency) {
  int n = pin1-pin0+1;
  int pins[n+1];
  for (int k=0; k<n; k++)
    pins[k] = pin0 + k;
  pins[n] = -1;
  setupTestSignals(pins, frequency);
}



Waveform *Waveform::WF = NULL;


Waveform::Waveform() {
  WF = this;
  Pin = -1;
  Rate = 0.0;
  MaxValue = 1 << 11;
  NHarmonics = 0;
  NData = 0;
  Data = NULL;
  Index = 0;
  setup(100000.0);
}


Waveform::~Waveform() {
  stop();
}


void Waveform::setup(float rate) {
  analogWriteResolution(12);
  MaxValue = 1 << 11;
  Rate = rate;
}


void Waveform::setHarmonics(float *ampls, float *phases) {
  for (NHarmonics=0; NHarmonics < MaxHarmonics &&
	 ampls[NHarmonics] > 0.0 && phases[NHarmonics] >= 0.0;
       NHarmonics++) {
    Ampls[NHarmonics] = ampls[NHarmonics];
    Phases[NHarmonics] = phases[NHarmonics];
  }
}


void Waveform::start(int pin, float freq, float ampl) {
  if ( Rate <= 0.0 ) {
    Serial.println("ERROR in Waveform::start(): no sampling rate specified.");
    return;
  }
#if defined(TEENSY32)
  if (pin != A14) {
    Serial.println("ERROR in Waveform::start(): invalid output pin specified.");
    return;
  }
#elif defined(TEENSYLC)
  if (pin != A12) {
    Serial.println("ERROR in Waveform::start(): invalid output pin specified.");
    return;
  }
#elif defined(TEENSY35) ||  defined(TEENSY36)
  if (pin != A21 && pin != A22) {
    Serial.println("ERROR in Waveform::start(): invalid output pin specified.");
    return;
  }
#else
  Serial.println("ERROR in Waveform::start(): DAC not supported by board.");
  return;
#endif
  Pin = pin;
  pinMode(Pin, OUTPUT);
  NData = round(Rate/freq);
  if ( NData < 4 )
    return;
  freq = Rate/NData;
  if ( Data != NULL )
    stop();
  // total amplitude:
  float amplt = 1.0;
  for (size_t j=0; j<NHarmonics; j++)
    amplt += Ampls[j];
  ampl /= amplt;
  // one period of data:
  Data = new uint16_t[NData];
  for ( size_t k=0; k<NData; k++) {
    float x = sin(TWO_PI*k/NData);
    for (size_t j=0; j<NHarmonics; j++)
      x += Ampls[j]*sin(TWO_PI*k*(j+2)/NData + Phases[j]);
    Data[k] = int16_t((MaxValue-1)*ampl*x) + MaxValue;
  }
  Index = 0;
  if ( !Timer.begin(write, 1.0e6/Rate) )
    Serial.printf("ERROR in Waveform::start(): failed to start timer at frequency %.1kHz\n", 0.001*Rate);
  else {
    char pins[4];
    analogPin(Pin, pins);
    Serial.printf("Generate %.0fHz waveform with %d samples on pin %s sampled with %.1fkHz.\n",
		  freq, NData, pins, 0.001*Rate);
  }
}


void Waveform::stop() {
  Timer.end();
  delete [] Data;
  Data = NULL;
  NData = 0;
  Index = 0;
}


void Waveform::write() {
  if (WF->Index >= WF->NData)
    WF->Index = 0;
  analogWrite(WF->Pin, WF->Data[WF->Index++]);
}
