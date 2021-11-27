/*
  Sensor - Virtual base class for environmental sensors.
  Created by Jan Benda, Novemeber 27th, 2021.
*/

#ifndef Sensor_h
#define Sensor_h


#include <Arduino.h>


class Sensor {

 public:

  float NoValue = -INFINITY;

  // Return name of environmental sensor reading as character array.
  virtual const char* parameter() const = 0;

  // Return unit of sensor readings as character array.
  virtual const char* unit() const = 0;

  // Return name of sensor chip as character array.
  virtual const char* chip() const = 0;

  // Return unique identifier as character array.
  virtual const char* identifier() const = 0;

  // Return true if sensor is available.
  virtual bool available() = 0;

  // Request a sensor reading.
  virtual void request() {};

  // Recommended delay between a request() and read().
  virtual unsigned long delay() const { return 0; };

  // Retrieve a sensor reading from the device.
  // You need to call request() at least delay() before.
  virtual void read() = 0;

  // The sensor reading.
  // On error, return -INFINITY.
  // Before you can retrieve a sensor reading,
  // you need to call request(), wait for at least delay() milliseconds,
  // and then call read().
  virtual float value() const = 0;
  
};


#endif
