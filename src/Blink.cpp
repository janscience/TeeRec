#include <Arduino.h>
#include "Blink.h"


Blink::Blink(int pin) {
  Pin = pin;
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);
  On= false;
  memset(Times, 0, sizeof(Times));
  Index = 0;
  State = 0;
}


Blink::~Blink() {
  for (int k=0; k<2; k++) {
    if (Times[k] != 0 )
      delete [] Times[k];
  }
  switchOff();
}


void Blink::set(uint32_t intervalms, uint32_t onms) {
  if (Times[0] != 0 )
    delete [] Times[0];
  Times[0] = new uint32_t[3];
  Times[0][0] = onms;
  Times[0][1] = intervalms - onms;
  Times[0][2] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::set(const uint32_t *times) {
  if (Times[0] != 0 )
    delete [] Times[0];
  int n = 0;
  while (times[n] > 0)
    n++;
  Times[0] = new uint32_t[n+1];
  for (int k=0; k<=n; k++)
    Times[0][k] = times[k];
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::blink(const uint32_t *times) {
  if (Times[1] != 0 )
    delete [] Times[1];
  int n = 0;
  while (times[n] > 0)
    n++;
  Times[1] = new uint32_t[n+1];
  for (int k=0; k<=n; k++)
    Times[1][k] = times[k];
  State = 1;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::switchOn(bool on) {
  if ( on != On ) {
    digitalWrite(Pin, on);
    On = on;
  }
}


void Blink::switchOff() {
  switchOn(false);
}


void Blink::update() {
  if (Times[State] != 0 && Time > Times[State][Index]) {
    Time -= Times[State][Index];
    switchOn(Index%2 == 0);
    Index++;
    if (Times[State][Index] == 0) {
      if (State > 0) {
	if (Times[1] != 0 )
	  delete [] Times[1];
	Times[1] = 0;
	State = 0;
      }
      Index = 0;
    }
  }
}



