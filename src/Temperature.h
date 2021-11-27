// on pin 10 (a 4.7K resistor is necessary)

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
