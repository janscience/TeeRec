/*
  Sensor - Virtual base class for environmental sensors.
  Created by Jan Benda, November 27th, 2021.
*/

#ifndef Sensor_h
#define Sensor_h


#include <Arduino.h>


class Sensors;


class Sensor {

 public:

  float NoValue = -INFINITY;

  // Initialize the sensor.
  Sensor();

  // Initialize the sensor and set name, mathematical symbol, basic unit, and
  // format of sensor readings.
  // The derived unit is also set to unit, and the conversion factor
  // is set to one.
  Sensor(const char *name, const char *symbol, const char *unit,
	 const char *format);

  // Initialize the sensor, add it to sensors, and set name,
  // mathematical symbol, basic unit, and format of sensor readings.
  // The derived unit is also set to unit, and the conversion factor
  // is set to one.
  Sensor(Sensors *sensors, const char *name, const char *symbol,
	 const char *unit, const char *format);

  // Return name of environmental sensor reading as character array.
  const char* name() const;

  // Set name of environmental sensor reading to name.
  void setName(const char *name);

  // Set name and mathematical symbol of environmental sensor reading.
  void setName(const char *name, const char *symbol);

  // Return mathematical symbol of environmental sensor reading as
  // character array.
  const char* symbol() const;

  // Set mathematical symbol of environmental sensor reading to name.
  void setSymbol(const char *symbol);

  // Return unit of sensor readings as character array.
  const char* basicUnit() const;

  // Return unit of derived sensor values as character array.
  const char* unit() const;

  // Set unit of environmental sensor reading to unit.
  // Optionally, the sensor reading can be mutliplied by fac to result
  // in the desired unit.
  void setUnit(const char *unit, float fac=1.0);

  // Set unit, conversion factor, and format string of environmental
  // sensor reading.
  void setUnit(const char *unit, float fac, const char *format);

  // Return format string for sensor readings as character array.
  const char* format() const;

  // Set format string for environmental sensor reading to format.
  void setFormat(const char *format);

  // Return name of sensor chip model as character array.
  virtual const char* chip() const;

  // Return unique identifier of sensor chip as character array.
  virtual const char* identifier() const;

  // Return resolution of the sensor readings in the current unit.
  // Any implementation should multiply the resolution with Fac
  // before returning the value.
  virtual float resolution() const = 0;
  
  // Print the resolution using format string into string s.
  // Return the number of printed characters.
  int resolutionStr(char *s) const;
  
  // Return true if sensor is available.
  virtual bool available() = 0;
  
  // Report properties of sensor on serial monitor.
  virtual void report();

  // Request a sensor reading.
  virtual void request() {};

  // Recommended delay between a request() and read().
  virtual unsigned long delay() const { return 0; };

  // Retrieve a sensor reading from the device.
  // You need to call request() at least delay() before.
  virtual void read() = 0;

  // The sensor reading in the basic unit.
  // On error, return -INFINITY.
  // Before you can retrieve a sensor reading,
  // you need to call request(), wait for at least delay() milliseconds,
  // and then call read().
  virtual float reading() const = 0;

  // The sensor reading in the current unit.
  // This default implementation multiplies the sensor reading() with Fac.
  virtual float value() const;
  
  // Print the sensor reading using format string into string s.
  // Return the number of printed characters.
  int valueStr(char *s) const;

  // Configure Sensor settings with the provided key-value pair.
  // Return true if key was used.
  virtual bool configure(const char *key, const char *val) { return false; };

  // Called from Sensors when this has been configured from a file, for example.
  void setConfigured() { Configured = true; };

  // True if this Sensor was configured from a file, for example.
  bool configured() const { return Configured; };

  
protected:

  char Name[50];
  char Symbol[20];
  char BasicUnit[20];
  char Unit[20];
  char Format[10];
  float Fac;
  bool Configured;
  
};


#endif
