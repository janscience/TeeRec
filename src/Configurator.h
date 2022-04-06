/*
  Configurator - configure configurable class instances from a configuration file.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Configurator_h
#define Configurator_h


#include <Arduino.h>


class Configurable;
class SDCard;


class Configurator {

 public:

  Configurator();

  void add(Configurable *config);

  Configurable *configurable(const char *name);

  void setConfigFile(const char *fname);

  void configure(SDCard &sd);

  bool configured() const { return Configured; };

  static Configurator *Config;

  
 private:

  static const size_t MaxConfigs = 10;
  size_t NConfigs;
  Configurable *Configs[MaxConfigs];

  static const size_t MaxFile = 100;
  char ConfigFile[MaxFile];

  bool Configured;

};



#endif
