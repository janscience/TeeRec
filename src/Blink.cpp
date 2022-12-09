#include <Arduino.h>
#include "Blink.h"


Blink::Blink() {
  Pin = -1;
  On = false;
  memset(Times, 0, sizeof(Times));
  Index = 0;
  State = 0;
  Delay = 0;
  Interval = 2000;
  OnTime = 50;
  OffTime = 150;
}


Blink::Blink(int pin) :
  Blink() {
  setPin(pin);
}


Blink::~Blink() {
  switchOff();
}


void Blink::setPin(int pin) {
  Pin = pin;
  if (Pin >= 0) {
    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);
  }
}


void Blink::setTiming(uint32_t intervalms, uint32_t onms, uint32_t offms) {
  Interval = intervalms;
  OnTime = onms;
  OffTime = offms;
}


void Blink::setSingle() {
  Times[0][0] = OnTime;
  Times[0][1] = Interval - OnTime;
  Times[0][2] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::setDouble() {
  Times[0][0] = OnTime;
  Times[0][1] = OffTime;
  Times[0][2] = OnTime;
  Times[0][3] = Interval - 2*OnTime - OffTime;
  Times[0][4] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::setTriple() {
  Times[0][0] = OnTime;
  Times[0][1] = OffTime;
  Times[0][2] = OnTime;
  Times[0][3] = OffTime;
  Times[0][4] = OnTime;
  Times[0][5] = Interval - 3*OnTime - 2*OffTime;
  Times[0][6] = 0;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::blinkSingle(uint32_t intervalms, uint32_t onms) {
  if (intervalms == 0)
    intervalms = Interval;
  if (onms == 0)
    onms = OnTime;
  Times[1][0] = onms;
  Times[1][1] = intervalms - onms;
  Times[1][2] = 0;
  State = 1;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::blinkDouble(uint32_t intervalms, uint32_t onms, uint32_t offms) {
  if (intervalms == 0)
    intervalms = Interval;
  if (onms == 0)
    onms = OnTime;
  if (offms == 0)
    offms = OffTime;
  Times[1][0] = onms;
  Times[1][1] = offms;
  Times[1][2] = onms;
  Times[1][3] = intervalms - 2*onms - offms;
  Times[1][4] = 0;
  State = 1;
  Index = 0;
  Time = 0;
  switchOn();
}


void Blink::blinkTriple(uint32_t intervalms, uint32_t onms, uint32_t offms) {
  if (intervalms == 0)
    intervalms = Interval;
  if (onms == 0)
    onms = OnTime;
  if (offms == 0)
    offms = OffTime;
  Times[1][0] = onms;
  Times[1][1] = offms;
  Times[1][2] = onms;
  Times[1][3] = offms;
  Times[1][4] = onms;
  Times[1][5] = intervalms - 3*onms - 2*offms;
  Times[1][6] = 0;
  State = 1;
  Index = 0;
  Time = 0;
  switchOn();
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


void Blink::clear() {
  Times[0][0] = 0;
  State = 0;
  Index = 0;
  Time = 0;
  switchOff();
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
  time = 0;
  while (time < delayms) {
    update();
  };
}


void Blink::switchOn(bool on) {
  if ( on != On ) {
    if (Pin >=0 )
      digitalWrite(Pin, on);
    On = on;
  }
}


void Blink::switchOff() {
  switchOn(false);
}

