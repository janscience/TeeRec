#include "Blink.h"


DigitalIODevice *Blink::InternIOs = 0;

volatile uint64_t Blink::PRNGState = 123456;


Blink::Blink(const char *name) :
  NPins(0),
  On(false),
  Delay(0),
  Random(false),
  Index(0),
  State(0),
  Interval(2000),
  OnTime(50),
  OffTime(150),
  NSwitchTimes(0),
  SwitchOverflow(false) {
  strncpy(Name, name, MaxName);
  Name[MaxName - 1] = '\0';
  memset(Times, 0, sizeof(Times));
}


Blink::Blink(const char *name, int pin1, bool invert1,
	     int pin2, bool invert2) :
  Blink(name) {
  if (pin1 >= 0)
    setPin(pin1, invert1);
  if (pin2 >= 0)
    setPin(pin2, invert2);
}


Blink::Blink(const char *name, int pin1, bool invert1,
	     DigitalIODevice &dev2, int pin2, bool invert2) :
  Blink(name) {
  if (pin1 >= 0)
    setPin(pin1, invert1);
  if (pin2 >= 0)
    setPin(dev2, pin2, invert2);
}


Blink::Blink(const char *name, DigitalIODevice &dev, int pin, bool invert) :
  Blink(name) {
  if (pin >= 0)
    setPin(dev, pin, invert);
}


Blink::Blink(const char *name, DigitalIODevice &dev1, int pin1, bool invert1,
	     DigitalIODevice &dev2, int pin2, bool invert2) :
  Blink(name) {
  if (pin1 >= 0)
    setPin(dev1, pin1, invert1);
  if (pin2 >= 0)
    setPin(dev2, pin2, invert2);
}


Blink::~Blink() {
  switchOff();
}


int8_t Blink::setPin(uint8_t pin, bool invert) {
  if (InternIOs == 0)
    InternIOs = new DigitalIODevice();
  return setPin(*InternIOs, pin, invert);
}


int8_t Blink::setPin(DigitalIODevice &device, uint8_t pin, bool invert) {
  if (NPins >= MaxPins)
    return -1;
  if (!device.available())
    return -1;
  Devices[NPins] = &device;
  Pins[NPins] = pin;
  Enabled[NPins] = true;
  Devices[NPins]->setMode(Pins[NPins], OUTPUT, invert);
  NPins += 1;
  On = true;
  switchOff();
  return NPins - 1;
}


void Blink::clearPins() {
  On = true;
  switchOff();
  NPins = 0;
}


void Blink::enablePin(uint8_t index, bool enable) {
  if (index >= NPins)
    return;
  if (Enabled[index] == enable)
    return;
  if (!enable)
    Devices[index]->write(Pins[index], LOW);
  Enabled[index] = enable;
}


void Blink::disablePin(uint8_t index) {
  enablePin(index, false);
}


void Blink::enablePins(bool enable) {
  for (uint8_t k=0; k<NPins; k++) {
    if (Enabled[k] == enable)
      continue;
    if (!enable)
      Devices[k]->write(Pins[k], LOW);
    Enabled[k] = enable;
  }
}


void Blink::disablePins() {
  enablePins(false);
}


void Blink::report(Stream &stream) const {
  if (NPins == 0)
    stream.print("no pins");
  else
    stream.printf("%d pin%s", NPins, NPins > 1 ? "s" : "");
  stream.printf(" initialized for %s indicator:\n", Name);
  for (uint8_t k=0; k<NPins; k++) {
    stream.printf("  pin %02d on device %s", Pins[k], Devices[k]->chip());
    if (Enabled[k])
      stream.println();
    else
      stream.println(" (disabled)");
  }
  stream.println();
}


void Blink::write(Stream &stream, size_t indent, size_t indent_incr) const {
  if (NPins == 0)
    return;
  stream.printf("%*s%s:\n", indent, "", Name);
  indent += indent_incr;
  for (uint8_t k=0; k<NPins; k++) {
    stream.printf("%*sP%d: pin %02d on device %s\n", indent, "",
		  k, Pins[k], Devices[k]->chip());
  }
}


void Blink::reset() {
  for (uint8_t k=0; k<NPins; k++)
    Devices[k]->setMode(Pins[k], OUTPUT);
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
  Random = false;
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


void Blink::setRandom(bool reset) {
  Random = true;
  Times[0][0] = OnTime + urand()*(OffTime - OnTime);
  Times[0][1] = OnTime + urand()*(OffTime - OnTime);
  Times[0][2] = 0;
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
  Random = false;
  Times[0][0] = onms;
  Times[0][1] = intervalms - onms;
  Times[0][2] = 0;
  Index = 0;
  if (reset)
    Time = 0;
  switchOn();
}


void Blink::set(const uint32_t *times, bool reset) {
  Random = false;
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
  Random = false;
  Times[0][0] = 0;
  State = 0;
  Index = 0;
  Time = 0;
  switchOff();
}


void Blink::update(bool allow_on) {
  if (Delay > 0) {
    if (Time > Delay and allow_on) {
      Time -= Delay;
      switchOn();
      Delay = 0;
    }
    return;
  }
  if (State == 0 && Index == 0 && Times[State][Index] == 0)
    return;
  if ((Time > Times[State][Index]) &&
      (((Index%2 == 1) && allow_on) || (Index%2 != 1))) {
    Time -= Times[State][Index];
    switchOn(Index%2 == 1);
    Index++;
    if (Times[State][Index] == 0) {
      if (State > 0) {
	Times[1][0] = 0;
	State = 0;
      }
      Index = 0;
      if (Random) {
	Times[0][0] = OnTime + urand()*(OffTime - OnTime);
	Times[0][1] = OnTime + urand()*(OffTime - OnTime);
      }
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


bool Blink::getSwitchTimes(uint32_t *times, bool *states, size_t *n) {
  memcpy(times, &SwitchTimes, NSwitchTimes*sizeof(uint32_t));
  memcpy(states, &SwitchStates, NSwitchTimes*sizeof(bool));
  *n = NSwitchTimes;
  NSwitchTimes = 0;
  bool overflow = SwitchOverflow;
  SwitchOverflow = false;
  return overflow;
}


void Blink::clearSwitchTimes() {
  NSwitchTimes = 0;
  SwitchOverflow = false;
}


void Blink::switchOn(bool on) {
  if (on != On) {
    for (uint8_t k=0; k<NPins; k++) {
      if (Enabled[k])
	Devices[k]->write(Pins[k], on);
    }
    if (NSwitchTimes < MaxTimes) {
      SwitchTimes[NSwitchTimes] = millis();
      SwitchStates[NSwitchTimes] = on;
      NSwitchTimes++;
    }
    else
      SwitchOverflow = true;
    On = on;
  }
}


void Blink::switchOff() {
  switchOn(false);
}


bool Blink::isOn() const {
  return On;
}


float Blink::urand(void) {
  // modified from https://forum.pjrc.com/threads/61125-Teensy-4-1-Random-Number-Generator?p=243895&viewfull=1#post243895
  uint64_t  x = PRNGState;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  PRNGState = x;
  return float((x * UINT64_C(2685821657736338717)) >> 32) / float(0xFFFFFFFF);
}
