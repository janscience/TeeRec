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
    SINGLEWIRE,
    ONEWIRE,
    I2C0,
    I2C1,
    I2C2,
    I2C3,
    SPI0,
    SPI1,
    SPI2
  };
  
  static const char *BusStrings[11];

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
  
  // Address on I2C bus as a string.
  virtual const char *addressStr() const { return AddressStr; };

  // Pin of OneWire bus or chip select pin of device on SPI bus.
  virtual int pin() const;
  
  // Pin as a string.
  virtual const char *pinStr() const { return PinStr; };

  // Return name of device (chip) model as character array.
  virtual const char* chip() const;

  // Return unique identifier of device as character array.
  virtual const char* identifier() const;

  // The number of key-value pairs available in the device's metadata.
  virtual size_t metadata() const { return NKeyVals; };

  // Return key of the index-th metadata entry.
  virtual const char *key(size_t index) const { return Keys[index]; };

  // Return value of the index-th metadata entry.
  virtual const char *value(size_t index) const { return Values[index]; };

  // Return true if device is available.
  // The default implementation returns the member variable Available.
  // So usually it is sufficient to set the Available variable once
  // when initializing the device.
  virtual bool available() const;
  
  // Report properties of device in one line on stream.
  virtual void report(Stream &stream=Serial);

  /* Write the device's chip and all the key-value pairs to stream (if available()). */
  virtual void write(Stream &stream=Serial, size_t indent=0, size_t indent_incr=4) const;

  
protected:

  // Set device type.
  void setDeviceType(const char *devicetype);

  // Set internal bus.
  void setInternBus();

  // Set SingleWire bus and its data pin.
  // Also set identifier to bus name plus data pin.
  void setSingleWireBus(int pin);

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

  // If key already exists, the set value of this key.
  // Otherwise, add a new key-value pair to the device's metadata.
  // The strings are not copied, only pointers are stored.
  // Make sure to pass in static strings.
  // Return the index of the set or added key-value pair.
  // If nothing was added, return -1.
  int add(const char *key, const char *value);

  // Set value of key-value pair at index.
  // Return true on success, i.e. when index is valid.
  bool setValue(size_t index, const char *value);

  // Set value of key-value pair.
  // Return the index of key on succes, otherwise -1.
  int setValue(const char *key, const char *value);

  BUS Bus;
  unsigned int Address;
  int Pin;
  static const int MaxType = 8;
  char DeviceType[MaxType];
  static const int MaxPin = 5;
  char AddressStr[MaxPin + 1];
  char PinStr[MaxPin + 1];
  static const int MaxStr = 32;
  char Chip[MaxStr];
  char Identifier[MaxStr];
  bool Available;
  
  static const size_t MaxKeyVals = 12;
  const char *Keys[MaxKeyVals];
  const char *Values[MaxKeyVals];
  size_t NKeyVals;
  
};


#endif
