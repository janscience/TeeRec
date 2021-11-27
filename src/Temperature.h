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


class Temperature {

 public:
  
  Temperature();
  Temperature(uint8_t pin);
  void begin(uint8_t pin);

  // Return ROM of temperature sensor (unique ID) as string.
  const char* address() const { return AddrS; };

  // Return name of chip as string.
  const char* chip() const { return Chip; };

  // Request a temperature conversion.
  void request();

  // Recommended delay between a request() and read().
  unsigned long delay() const { return 1000; };

  // Retrieve a temperature reading from the device.
  // You need to call request() at least 1sec before.
  void read();

  // The temperature in degrees celsius.
  // On error, return -1000.
  // Before you can retrieve a temperature reading,
  // you need to call request(), wait for at least delay() milliseconds,
  // and then call read().
  float temperature() const { return Celsius; };
  
  
 private:

  OneWire  OW;
  int Type_s;
  byte Addr[8];
  char AddrS[25];
  char Chip[8];
  float Celsius;
};


#endif
