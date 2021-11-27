/*
  Temperature - read temperature from a 1-wire device.
  Created by Jan Benda, Novemeber 27th, 2021.
  Based on https://github.com/PaulStoffregen/OneWire/blob/master/examples/DS18x20_Temperature/DS18x20_Temperature.pde

  The temperature sensor needs to be connected to the Teensy in the
  following way:
  - GND to Teensy GND
  - power to Teensy 3.3V
  - DATA to a Teensy digital output pin (e.g. pin 10).
    DATA also needs to be connected to Teensy 3.3V via a 4.7K resistor.

  See
  https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
  step 4 for wiring.

  More resourves on the OneWire protocol can be found here:
  https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
*/

#ifndef Temperature_h
#define Temperature_h


#include <Arduino.h>
#include <OneWire.h>
#include <Sensor.h>


class Temperature : public Sensor {

 public:
  
  Temperature();
  Temperature(uint8_t pin);
  void begin(uint8_t pin);

  // Return "temperature".
  virtual const char* parameter() const { return "temperature"; };

  // Return unit of temperature readings: "celsius".
  virtual const char* unit() const { return "celsius"; };

  // Return name of chip as string.
  virtual const char* chip() const { return Chip; };

  // Return ROM of temperature sensor (unique ID) as string.
  virtual const char* identifier() const { return AddrS; };

  // Return true if temperature sensor is available.
  virtual bool available();

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
  virtual float value() const { return Celsius; };
  
  
 private:

  OneWire  OW;
  int Type_s;
  byte Addr[8];
  char AddrS[25];
  char Chip[8];
  float Celsius;
};


#endif
