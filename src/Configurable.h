/*
  Configurable - base class for all configurable classes.
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Configurable_h
#define Configurable_h


#include <Arduino.h>


class Configurable {

 public:

  // Initialize configuration section name.
  Configurable(const char *name);

  // All lower case name of the configurable.
  const char *name() const { return ConfigName; };

  // Set name of the configurable to name.
  void setName(const char *name);

  // Configure the class with the provided key-value pair.
  virtual void configure(const char *key, const char *val);

  // Returns time in seconds from a string with unit (ms, s, min, or h).
  static float parseTime(const char *val);

  // Returns frequency in Hertz from a string with unit (Hz, kHz, MHz, or mHz).
  static float parseFrequency(const char *val);

  // Called from Configurator when this configurable is configured.
  void setConfigured() { Configured = true; };

  // True if this was configured from a file, for example.
  bool configured() const { return Configured; };


 private:

  static const size_t MaxName = 100;
  char ConfigName[MaxName];
  bool Configured;

};


#endif
