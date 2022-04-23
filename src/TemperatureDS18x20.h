/*
  TemperatureDS18x20 - read temperature from a DS18x20 1-wire device.
  Created by Jan Benda, November 27th, 2021.
  Based on https://github.com/PaulStoffregen/OneWire/blob/master/examples/DS18x20_Temperature/DS18x20_Temperature.pde

  The temperature sensor needs to be connected to the Teensy in the
  following way:
  - black: GND to Teensy GND
  - black: power to Teensy 3.3V
  - yellow: DATA to a Teensy digital output pin (e.g. pin 10).
    DATA also needs to be connected to Teensy 3.3V via a 4.7K resistor.

  See
  https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
  step 4 for wiring.

  More resources on the OneWire protocol can be found here:
  https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806

  The [DS18B20 data
  sheet](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf).
*/

#ifndef TemperatureDS18x20_h
#define TemperatureDS18x20_h


#include <Arduino.h>
#include <OneWire.h>
#include <Sensor.h>


class TemperatureDS18x20 : public Sensor {

 public:

  // Do not initialize temperature device yet.
  TemperatureDS18x20();
  
  // Initialize temperature device with DATA on pin.
  TemperatureDS18x20(uint8_t pin);
  
  // Initialize temperature device with DATA on pin.
  void begin(uint8_t pin);

  // Return name of chip as string.
  const char* chip() const { return Chip; };

  // Return ROM of temperature sensor (unique ID) as string.
  const char* identifier() const { return AddrS; };

  // Return resolution of the temperature readings.
  virtual float resolution() const;

  // Return true if temperature sensor is available.
  virtual bool available();

  // Report temperature device on serial monitor.
  virtual void report();

  // Request a temperature conversion.
  virtual void request();

  // Recommended delay between a request() and read() in milliseconds.
  virtual unsigned long delay() const { return 1000; };

  // Retrieve a temperature reading from the device.
  // You need to call request() at least delay() milliseconds before.
  virtual void read();

  // The temperature in degrees celsius.
  // On error, return -INFINITY.
  // Before you can retrieve a temperature reading,
  // you need to call request(), wait for at least delay() milliseconds,
  // and then call read().
  virtual float value() const;

  // Configure Temperature settings with the provided key-value pair.
  // Return true if key was used.
  // If key equals "ds18x20-pin", initialize temperature device on
  // specified pin. If value is negative or "none", do not initialize.
  virtual bool configure(const char *key, const char *val);
  
  
 private:

  OneWire  OW;
  int Type_s;
  byte Addr[8];
  char AddrS[25];
  char Chip[8];
  float Celsius;
};


#endif