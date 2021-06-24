#include <Arduino.h>
#include "Blink.h"


Blink::Blink(int pin) {
  Pin = pin;
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);
  Status= false;
  OnTime = 0;
  OffTime = 0;
}


Blink::~Blink() {
  if ( Status )
    digitalWrite(Pin, LOW);
}


void Blink::setTiming(uint32_t intervalms, uint32_t onms) {
  OnTime = onms;
  OffTime = intervalms - onms;
  Time = 0;
}


void Blink::set(bool on) {
  if ( on != Status ) {
    digitalWrite(Pin, on);
    Status = on;
  }
}


void Blink::update() {
  if ( Status && OnTime > 0 && Time > OnTime ) {
    set(false);
    Time -= OnTime;
  }
  if ( ! Status && OffTime > 0 && Time > OffTime ) {
    set(true);
    Time -= OffTime;
  }
}



