/*
  DigitalIOPCA9536 - Controling the PCA9536 digital input/output device.
  Created by Jan Benda, August 22th, 2025.
*/

#ifndef DigitalIOPCA9536_h
#define DigitalIOPCA9536_h


#include <Wire.h>
#include <DigitalIODevice.h>


class DigitalIOPCA9536: public DigitalIODevice {

 public:

  // Initialize the PCA9536 digital IO device on I2C bus.
  DigitalIOPCA9536(TwoWire &wire=Wire);
  
  // Initialize PCA9536 digital IO device on I2C bus.
  void begin(TwoWire &wire=Wire);

  // Set mode (INPUT, OUTPUT) for pin.
  virtual void setMode(uint8_t pin, uint8_t mode);

  // Read val (LOW or HIGH) from pin.
  virtual uint8_t read(uint8_t pin);

  // Write val (LOW or HIGH) to pin.
  virtual void write(uint8_t pin, uint8_t val);

  
 private:
  
  TwoWire  *I2CBus;
  uint8_t Mode;
  uint8_t Bits;
  
};


#endif
