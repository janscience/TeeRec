/*
  Sensors - Manage environmental sensors.
  Created by Jan Benda, November 27th, 2021.
*/

#ifndef Sensors_h
#define Sensors_h


#include <Arduino.h>
#include <Configurable.h>
#include <Sensor.h>
#include <SDWriter.h>
#include <RTClock.h>


class Sensors : public Configurable {

 public:

  Sensors();

  // Add a sensor if available.
  void addSensor(Sensor &sensor);

  // Number of currently managed sensors.
  uint8_t size() const { return NSensors; };

  // The index-th sensor.
  Sensor &operator[](uint8_t index) { return *Snsrs[index]; };

  // Set number of csv files to be written.
  void setNFiles(int nfiles);

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

  // Return true if sensor readings are pending.
  // The pending state is cleared by this function.
  bool pending();

  // Report all sensor readings on serial monitor.
  void print();

  // Open csv files for sensor readings to path on SD card sd
  // and write header line.
  // path is without extension. 'csv' is added.
  // NFiles csv files are opened.
  // If nfiles is greater than one, a number is added to path.
  // If append and path already exists, then keep the file
  // and do not write the header.
  // Return true on success.
  // You can open up to MaxFiles files by calling this function
  // repeatedly with different pathes.
  bool openCSV(SDCard &sd, const char *path, RTClock &rtc,
	       bool append=false);

  // Write current time and sensor readings to csv file.
  // Return true on success.
  bool writeCSV();

  // Configure Sensor settings with the provided key-value pair.
  virtual void configure(const char *key, const char *val);

  
 private:

  static const uint8_t MaxSensors = 10; 
  uint8_t NSensors; 
  Sensor *Snsrs[MaxSensors];
  unsigned long MaxDelay;
  unsigned long Interval;
  unsigned long UseInterval;
  elapsedMillis Time;
  int State;
  bool Pending;
  static const uint8_t MaxFiles = 5; 
  uint8_t NFiles;   // number of open csv files
  uint8_t CFile;    // index of csv file to be written next
  FsFile DF[MaxFiles];
  RTClock *RTC;
};


#endif
