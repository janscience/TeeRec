#include <Arduino.h>
#include "ContinuousADC.h"
#include "Waveform.h"

Waveform *Waveform::WF = NULL;


Waveform::Waveform() {
  WF = this;
  Pin = -1;
  Rate = 0.0;
  NData = 0;
  Data = NULL;
  Index = 0;
}


Waveform::~Waveform() {
  stop();
}


void Waveform::setup(int pin, float rate) {
  Pin = pin;
  analogWriteResolution(16);
  pinMode(Pin, OUTPUT);
  analogWrite(Pin, 0);
  Rate = rate;
}


void Waveform::start(float freq, float ampl) {
  if ( Rate <= 0.0 )
    return;
  NData = round(Rate/freq);
  if ( NData < 4 )
    return;
  freq = Rate/NData;
  if ( Data != NULL )
    stop();
  Data = new uint16_t[NData];
  for ( size_t k=0; k<NData; k++)
    Data[k] = int16_t(32767*ampl*sin(TWO_PI*freq*k/NData)) + 32768;
  Index = 0;
  if ( !Timer.begin(write, 1.0e6/Rate) )
    Serial.printf("failed to start timer at frequency %.1kHz\n", 0.001*Rate);
  else {
    char pins[4];
    analogPin(Pin, pins);
    Serial.printf("Generate %.0fHz waveform on pin %s sampled with %.1fkHz\n",
		  freq, pins, 0.001*Rate);
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


