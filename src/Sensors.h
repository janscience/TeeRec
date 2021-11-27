/*
  Sensors - Manage environmental sensors.
  Created by Jan Benda, November 27th, 2021.
*/

#ifndef Sensors_h
#define Sensors_h


#include <Arduino.h>
#include <Sensor.h>
#include <SDWriter.h>
#include <RTClock.h>


class Sensors {

 public:

  Sensors();

  // Add a sensor if available.
  void addSensor(Sensor &sensor);

  // Number of currently managed sensors.
  uint8_t size() const { return NSensors; };

  // The index-th sensor.
  Sensor &operator[](uint8_t index) { return *Snsrs[index]; };

  // Set update interval for reading sensor values to interval seconds.
  void setInterval(float interval);

  // Report all sensor devices on serial monitor.
  void report();

  // Start acquisition of sensor values.
  void start();

  // Update sensor readings.
  // Call as often as possible in loop().
  // Returns true if the sensor readings have been updated.
  bool update();

  // Report all sensor readings on serial monitor.
  void print();

  // Write csv for sensor readings to path on SD card sd.
  // Use rtc for getting time of sensor readings.
  // If append and path already exists, then keep the file
  // and do not write the header.
  // Return on success.
  bool writeCSVHeader(SDCard &sd, const char *path, RTClock &rtc,
		      bool append=true);

  // Write current sensor readings to csv file.
  // Return true on success.
  bool writeCSV();

 
 private:

  static const uint8_t MaxSensors = 10; 
  uint8_t NSensors; 
  Sensor *Snsrs[MaxSensors];
  unsigned long MaxDelay;
  unsigned long Interval;
  elapsedMillis Time;
  int State;
  SDCard *SDC;
  char Path[100];
  RTClock *RTC;
};


#endif
