#include <PCA9536DigitalIO.h>


#define PCA9536_ADDR 0x82


PCA9536DigitalIO::PCA9536DigitalIO(TwoWire &wire) :
  DigitalIODevice() {
  setChip("PCA9536");
  begin(wire);
}


void PCA9536DigitalIO::begin(TwoWire &wire) {
  Mode = 0xff;
  Bits = 0;
  I2CBus = &wire;
  setI2CBus(wire, PCA9536_ADDR);
}


bool PCA9536DigitalIO::available() const {
  I2CBus->beginTransmission(PCA9536_ADDR);
  return (I2CBus->endTransmission() == 0);
}


void PCA9536DigitalIO::setMode(uint8_t pin, uint8_t mode) {
  if (mode == INPUT)
    Mode |= 1 << pin;
  else
    Mode &= ~(1 << pin);
  I2CBus->beginTransmission(PCA9536_ADDR);
  I2CBus->write((uint8_t)0x03); 
  I2CBus->write(Mode); 
  I2CBus->endTransmission();
}


uint8_t PCA9536DigitalIO::read(uint8_t pin) {
  I2CBus->beginTransmission(PCA9536_ADDR);
  I2CBus->write((uint8_t)0x00); 
  if (I2CBus->endTransmission() == 0) {
    I2CBus->requestFrom(PCA9536_ADDR, 1);
    if (I2CBus->available() == 1) {
      unt8_t x = I2CBus->read();
      return ((x & (1 << pin)) > 0) != Inverted[pin];
    }
  }
  return 0;
}


void PCA9536DigitalIO::write(uint8_t pin, uint8_t val) {
  if (val != Inverted[pin])
    Bits |= 1 << pin;
  else
    Bits &= ~(1 << pin);
  I2CBus->beginTransmission(PCA9536_ADDR);
  I2CBus->write((uint8_t)0x01); 
  I2CBus->write(Bits); 
  I2CBus->endTransmission();
}

