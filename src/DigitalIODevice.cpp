#include <DigitalIODevice.h>


DigitalIODevice::DigitalIODevice() :
  Device() {
  setDeviceType("gpio");
  setInternBus();
  setChip("Teensy");
  memset(Inverted, 0, sizeof(Inverted));
}


bool DigitalIODevice::available() const {
  return true;
}


void DigitalIODevice::setMode(uint8_t pin, uint8_t mode, bool inverted) {
  if (pin < MaxPins) {
    Inverted[pin] = inverted;
    setMode(pin, mode);
  }
}


void DigitalIODevice::setMode(uint8_t pin, uint8_t mode) {
  pinMode(pin, mode);
}


uint8_t DigitalIODevice::read(uint8_t pin) {
  return digitalRead(pin) != Inverted[pin];
}


void DigitalIODevice::write(uint8_t pin, uint8_t val) {
  digitalWrite(pin, val != Inverted[pin]);
}

