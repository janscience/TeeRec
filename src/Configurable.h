/*
  Configurable - make a class configurable.
  Created by Jan Benda, May 26th, 2021.
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

  // Configure the class with the provided key-value pair.
  virtual void configure(const char *key, const char *val);


 private:

  char *ConfigName;

};


#endif
