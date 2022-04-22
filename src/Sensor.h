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
  Sensor();

  // Initialize the sensor and set name, unit, and format of sensor readings.
  Sensor(const char *name, const char *unit, const char *format);

  // Return name of environmental sensor reading as character array.
  const char* name() const;

  // Set name of environmental sensor reading to name.
  void setName(const char *name);

  // Return unit of sensor readings as character array.
  const char* unit() const;

  // Set unit of environmental sensor reading to unit.
  void setUnit(const char *unit);

  // Return format string for sensor readings as character array.
  const char* format() const;

  // Set format string for environmental sensor reading to format.
  void setFormat(const char *format);

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
  
  // Print the sensor reading using format string into string s.
  // Return the number of printed characters.
  int print(char *s) const;

  // Configure Sensor settings with the provided key-value pair.
  // Return true if key was used.
  virtual bool configure(const char *key, const char *val) { return false; };

  // Called from Sensors when this has been configured from a file, for example.
  void setConfigured() { Configured = true; };

  // True if this Sensor was configured from a file, for example.
  bool configured() const { return Configured; };

  
private:

  char Name[50];
  char Unit[50];
  char Format[10];
  bool Configured;
  
};


#endif
