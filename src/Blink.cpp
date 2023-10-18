#include <Arduino.h>
#include "Blink.h"


Blink::Blink() {
  Pin1 = -1;
  Pin2 = -1;
  Invert = false;
  On = false;
  memset(Times, 0, sizeof(Times));
  Index = 0;
  State = 0;
  Delay = 0;
  Interval = 2000;
  OnTime = 50;
  OffTime = 150;
}


Blink::Blink(int pin1, bool invert1, int pin2, bool invert2) :
  Blink() {
  setPin(pin1, invert1);
  setPin2(pin2, invert2);
}


Blink::~Blink() {
  switchOff();
}


void Blink::setPin(int pin, bool invert) {
  Pin1 = pin;
  Invert = invert;
  if (Pin1 >= 0) {
    pinMode(Pin1, OUTPUT);
    switchOff();
  }
}


void Blink::setPin2(int pin, bool invert) {
  Pin2 = pin;
  Invert = invert;
  if (Pin2 >= 0) {
    pinMode(Pin2, OUTPUT);
    switchOff();
  }
}


void Blink::setTiming(uint32_t intervalms, uint32_t onms, uint32_t offms) {
  Interval = intervalms;
  OnTime = onms;
  OffTime = offms;
}


void Blink::setSingle(bool reset) {
  setMultiple(1, reset);
}


void Blink::setDouble(bool reset) {
  setMultiple(2, reset);
}


void Blink::setTriple(bool reset) {
  setMultiple(3, reset);
}


void Blink::setMultiple(int n, bool reset) {
  for (int k=0; k<n; k++) {
    Times[0][2*k+0] = OnTime;
    Times[0][2*k+1] = OffTime;
  }
  if (Interval > n*OnTime + (n-1)*OffTime)
    Times[0][2*n-1] = Interval - n*OnTime - (n-1)*OffTime;
  else
    Times[0][2*n-1] = 0;
  Times[0][2*n] = 0;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::blinkSingle(uint32_t intervalms, uint32_t onms, bool reset) {
  blinkMultiple(1, intervalms, onms, OffTime, reset);
}


void Blink::blinkDouble(uint32_t intervalms, uint32_t onms, uint32_t offms,
			bool reset) {
  blinkMultiple(2, intervalms, onms, offms, reset);
}


void Blink::blinkTriple(uint32_t intervalms, uint32_t onms, uint32_t offms,
			bool reset) {
  blinkMultiple(3, intervalms, onms, offms, reset);
}


void Blink::blinkMultiple(int n, uint32_t intervalms, uint32_t onms,
			  uint32_t offms, bool reset) {
  if (intervalms == 0)
    intervalms = Interval;
  if (onms == 0)
    onms = OnTime;
  if (offms == 0)
    offms = OffTime;
  for (int k=0; k<n; k++) {
    Times[1][2*k+0] = onms;
    Times[1][2*k+1] = offms;
  }
  if (intervalms > n*onms + (n-1)*offms)
    Times[1][2*n-1] = intervalms - n*onms - (n-1)*offms;
  else
    Times[1][2*n-1] = 0;
  Times[1][2*n] = 0;
  State = 1;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::set(uint32_t intervalms, uint32_t onms, bool reset) {
  Times[0][0] = onms;
  Times[0][1] = intervalms - onms;
  Times[0][2] = 0;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::set(const uint32_t *times, bool reset) {
  int n = 0;
  for ( ;n<MaxTimes-1 && times[n] != 0; n++)
    Times[0][n] = times[n];
  Times[0][n] = 0;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::setDelayed(uint32_t delayms, uint32_t intervalms, uint32_t onms,
		       bool reset) {
  set(intervalms, onms, reset);
  Delay = delayms;
  if (Delay > 0)
    switchOff();
}


void Blink::setDelayed(uint32_t delayms, const uint32_t *times, bool reset) {
  set(times, reset);
  Delay = delayms;
  if (Delay > 0)
    switchOff();
}


void Blink::blink(uint32_t intervalms, uint32_t onms, bool reset) {
  Times[1][0] = onms;
  Times[1][1] = intervalms - onms;
  Times[1][2] = 0;
  State = 1;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::blink(const uint32_t *times, bool reset) {
  int n = 0;
  for ( ;n<MaxTimes-1 && times[n] != 0; n++)
    Times[1][n] = times[n];
  Times[1][n] = 0;
  State = 1;
  Index = 0;
  if (reset)
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
      Time -= Delay;
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
    yield();
  };
}


void Blink::switchOn(bool on) {
  if (on != On) {
    if (Pin1 >=0)
      digitalWrite(Pin1, Invert != on);
    if (Pin2 >=0)
      digitalWrite(Pin2, Invert != on);
    On = on;
  }
}


void Blink::switchOff() {
  switchOn(false);
}


/////////////////// Random numbers //////////////////////////////////////

// from https://forum.pjrc.com/threads/61125-Teensy-4-1-Random-Number-Generator?p=243895&viewfull=1#post243895 :

static volatile uint64_t  prng_state;  /* Any nonzero state is valid! */

uint32_t prng_u32(void) {
  uint64_t  x = prng_state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  prng_state = x;
  return (x * UINT64_C(2685821657736338717)) >> 32;
}

// Normal distrubuted random nunbers, see
// https://forum.pjrc.com/threads/71225-Gaussian-Distributed-Random-Numbers-for-Teensy-4-1?p=313878&viewfull=1#post313878
