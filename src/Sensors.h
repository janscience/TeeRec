/*
  Sensors - Manage environmental sensors.
  Created by Jan Benda, Novemeber 27th, 2021.
*/

#ifndef Sensors_h
#define Sensors_h


#include <Arduino.h>
#include <Sensor.h>


class Sensors {

 public:

  Sensors();

  // Add a sensor if available.
  void addSensor(Sensor *sensor);

  // Number of currently managed sensors.
  uint8_t size() const { return NSensors; };

  // The index-th sensor.
  Sensor &operator[](uint8_t index) { return *Snsrs[index]; };

  // Set update interval for reading sensor values to interval milliseconds.
  void setInterval(unsigned long interval);

  // Start acquisition of sensor values.
  void start();

  // Update sensor readings.
  // Call as often as possible in loop().
  // Returns true if the sensor readings have been updated.
  bool update();

  // Report all sensor readings on serial monitor.
  void report();

  
 private:

  static const uint8_t MaxSensors = 10; 
  uint8_t NSensors; 
  Sensor *Snsrs[MaxSensors];
  unsigned long MaxDelay;
  unsigned long Interval;
  elapsedMillis Time;
  int State;
  
};


#endif
