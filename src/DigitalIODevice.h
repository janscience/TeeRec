/*
  DigitalIODevice - Base class for operating a digital input/output devices.
  Created by Jan Benda, August 22th, 2025.

  The default implementation accesses the microprocessors internal pins.
*/

#ifndef DigitalIODevice_h
#define DigitalIODevice_h


#include <Device.h>


#ifndef LOW
#define LOW 0x0
#endif

#ifndef HIGH
#define HIGH 0x1
#endif


class DigitalIODevice: public Device {

 public:

  // Initialize the DigitalIO device (the microcontroller itself).
  DigitalIODevice();

  // Return true if device is available.
  virtual bool available() const;

  // set mode (INPUT, INPUT_PULLUP, INPUT_PULLDOWN, OUTPUT, OUTPUT_OPENDRAIN)
  // for pin.
  // If inverted, then invert the meaning of LOW and HIGH for this pin.
  void setMode(uint8_t pin, uint8_t mode, bool inverted);

  // set mode (INPUT, INPUT_PULLUP, INPUT_PULLDOWN, OUTPUT, OUTPUT_OPENDRAIN)
  // for pin.
  // Do not change previously set invertedness.
  virtual void setMode(uint8_t pin, uint8_t mode);

  // read val (LOW or HIGH) from pin.
  virtual uint8_t read(uint8_t pin);

  // write val (LOW or HIGH) to pin.
  virtual void write(uint8_t pin, uint8_t val);
  

 protected:

  static const uint8_t MaxPins = 48;
  bool Inverted[MaxPins];
  
};


#endif
