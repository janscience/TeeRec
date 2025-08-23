/*
  Device - Virtual base class describing any device.
  Created by Jan Benda, January 6th, 2025.
*/

#ifndef Device_h
#define Device_h


#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


class Device {

 public:

  enum BUS {
    UNKNOWN,
    INTERN,
    ONEWIRE,
    I2C0,
    I2C1,
    I2C2,
    I2C3,
    SPI0,
    SPI1,
    SPI2
  };
  
  static const char *BusStrings[10];

  // Initialize the device.
  Device();
  
  // Type of device (max 8 characters).
  const char *deviceType() const { return DeviceType; };

  // Bus controlling the device.
  virtual BUS bus() const;

  // Bus controlling the device as a string (max 8 characters).
  const char *busStr() const { return BusStrings[bus()]; };

  // Address on I2C bus.
  virtual unsigned int address() const;

  // Pin of OneWire bus or chip select pin of device on SPI bus.
  virtual int pin() const;

  // Return name of device (chip) model as character array.
  virtual const char* chip() const;

  // Return unique identifier of device as character array.
  virtual const char* identifier() const;

  // Return true if device is available.
  // The default implementation returns the member variable Available.
  // So usually it is sufficient to set the Available variable once
  // when initializing the device.
  virtual bool available() const;
  
  // Report properties of device on stream.
  virtual void report(Stream &stream=Serial);

  
protected:

  // Set device type.
  void setDeviceType(const char *devicetype);

  // Set internal bus.
  void setInternBus() { Bus = BUS::INTERN; };

  // Set OneWire bus and its data pin.
  void setOneWireBus(int pin);

  // Set I2C bus and address that controls the chip.
  // Also set identifier to bus name plus I2C address.
  void setI2CBus(const TwoWire &wire, unsigned int address);

  // Set SPI bus and CS pin that controls the chip.
  // Also set identifier to bus name plus CS pin.
  void setSPIBus(const SPIClass &spi, unsigned int cspin);

  // Set the name of the chip.
  void setChip(const char *chip);

  // Set the unique identifier of the chip.
  void setIdentifier(const char *identifier);

  BUS Bus;
  unsigned int Address;
  int Pin;
  static const int MaxType = 8;
  char DeviceType[MaxType];
  static const int MaxStr = 32;
  char Chip[MaxStr];
  char Identifier[MaxStr];
  bool Available;
  
};


#endif
