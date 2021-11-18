#include <Arduino.h>
#include "Blink.h"


Blink::Blink(int pin) {
  Pin = pin;
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);
  On = false;
  memset(Times, 0, sizeof(Times));
  Index = 0;
  State = 0;
  Delay = 0;
}


Blink::~Blink() {
  switchOff();
}


void Blink::set(uint32_t intervalms, uint32_t onms) {
  Times[0][0] = onms;
  Times[0][1] = intervalms - onms;
  Times[0][2] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::set(const uint32_t *times) {
  int n = 0;
  for ( ;n<MaxTimes-1 && times[n] != 0; n++)
    Times[0][n] = times[n];
  Times[0][n] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::setDelayed(uint32_t delayms, uint32_t intervalms, uint32_t onms) {
  set(intervalms, onms);
  Delay = delayms;
  if (Delay > 0)
    switchOff();
}


void Blink::setDelayed(uint32_t delayms, const uint32_t *times) {
  set(times);
  Delay = delayms;
  if (Delay > 0)
    switchOff();
}


void Blink::clear() {
  Times[0][0] = 0;
  Index = 0;
  Time = 0;
  switchOff();
}


void Blink::blink(uint32_t intervalms, uint32_t onms) {
  Times[1][0] = onms;
  Times[1][1] = intervalms - onms;
  Times[1][2] = 0;
  State = 1;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::blink(const uint32_t *times) {
  int n = 0;
  for ( ;n<MaxTimes-1 && times[n] != 0; n++)
    Times[1][n] = times[n];
  Times[1][n] = 0;
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
  if (Delay > 0) {
    if (Time > Delay) {
      Time = 0;
      switchOn();
      Delay = 0;
    }
    return;
  }
  if (State == 0 && Index == 0 && Times[State][Index] == 0)
    return;
  if (Time > Times[State][Index]) {
    Time -= Times[State][Index];
    switchOn(Index%2 == 1);
    Index++;
    if (Times[State][Index] == 0) {
      if (State > 0) {
	Times[1][0] = 0;
	State = 0;
      }
      Index = 0;
    }
  }
}


void Blink::delay(uint32_t delayms) {
  elapsedMillis time;
  while (time < delayms) {
    update();
  };
}

