/*
  Temperature - read temperature from a 1-wire device.
  Created by Jan Benda, November 27th, 2021.
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

  // Do not initialize temperature device yet.
  Temperature();
  
  // Initialize temperature device with DATA on pin.
  Temperature(uint8_t pin);
  
  // Initialize temperature device with DATA on pin.
  void begin(uint8_t pin);

  // Return name of chip as string.
  const char* chip() const { return Chip; };

  // Return ROM of temperature sensor (unique ID) as string.
  const char* identifier() const { return AddrS; };

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
  virtual float value() const { return Celsius; };

  // Print the temperature in degrees celsius into string s.
  // Return the number of printed characters.
  virtual int print(char *s) const;
  
  
 private:

  OneWire  OW;
  int Type_s;
  byte Addr[8];
  char AddrS[25];
  char Chip[8];
  float Celsius;
};


#endif
