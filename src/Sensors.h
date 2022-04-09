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
  Sensors(RTClock &rtc);
  ~Sensors();

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
  // You need to start the acquisition before by calling start()
  // Returns true if the sensor readings have been updated.
  bool update();

  // Return true if sensor readings are pending.
  // The pending state is cleared by this function.
  bool pending();

  // Report all sensor readings on serial monitor.
  void print();

  // Pass real-time clock to Sensors, needed by writeCSV().
  void setRTClock(RTClock &rtc);
  
  // Create header line for CSV file.
  // Usually, this is automatically called by openCSV().
  void makeCSVHeader();

  // Open csv file for sensor readings at path on SD card sd
  // and write header line.
  // If no header line was ever created, makeCSVHeader() is called.
  // path is without extension. 'csv' is added.
  // If append and path already exists, then keep the file
  // and do not write the header.
  // Return true on success, false on failure or no available sensors.
  bool openCSV(SDCard &sd, const char *path, bool append=false);

  // Write current time and sensor readings to csv file.
  // Return true on success, false on failure or if file is not open
  // for writing.
  bool writeCSV();

  // Close csv file.
  // Return true on success.
  bool closeCSV();

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
  FsFile DF;
  char *Header;
  RTClock *RTC;
};


#endif
