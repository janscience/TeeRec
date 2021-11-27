/*
  Sensor - Virtual base class for environmental sensors.
  Created by Jan Benda, November 27th, 2021.
*/

#ifndef Sensor_h
#define Sensor_h


#include <Arduino.h>


class Sensor {

 public:

  float NoValue = -INFINITY;

  // Initialize the sensor.
  Sensor() { Name[0] = '\0'; Unit[0] = '\0'; };

  // Initialize the sensor and set name and unit of sensor readings.
  Sensor(const char *name, const char *unit) { setName(name); setUnit(unit); };

  // Return name of environmental sensor reading as character array.
  const char* name() const { return Name; };

  // Set name of environmental sensor reading to name.
  void setName(const char *name) { strcpy(Name, name); };

  // Return unit of sensor readings as character array.
  const char* unit() const { return Unit; };

  // Set unit of environmental sensor reading to unit.
  void setUnit(const char *unit) { strcpy(Unit, unit); };

  // Return resolution of the sensor readings.
  virtual float resolution() const = 0;
  
  // Return true if sensor is available.
  virtual bool available() = 0;
  
  // Report sensor device on serial monitor.
  virtual void report() {};

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
  
  // Print the sensor reading into string s.
  // Return the number of printed characters.
  virtual int print(char *s) const = 0;


private:

  char Name[50];
  char Unit[50];
  
};


#endif
