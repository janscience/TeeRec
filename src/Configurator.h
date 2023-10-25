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

  /* Initialize. */
  Configurator();

  /* Add config to this Configurator. */
  void add(Configurable *config);

  /* Return pointer to Configurable instance with given name. */
  Configurable *configurable(const char *name);

  /* Name of the configuration file. */
  void setConfigFile(const char *fname);

  /* Report parameters of all configurables on serial. */
  void report() const;

  /* Read configuration file from SD card and pass key-value pairs to
     the Configurables. */
  void configure(SDCard &sd);

  /* Write current setting to configuration file on SD card.
     Return true on success. */
  bool save(SDCard &sd) const;

  /* True if the configuration file has been read. */
  bool configured() const { return Configured; };

  static Configurator *Config;

  
 private:

  static const size_t MaxConfigs = 10;
  size_t NConfigs;
  Configurable *Configs[MaxConfigs];

  static const size_t MaxFile = 99;
  char ConfigFile[MaxFile + 1];

  bool Configured;

};



#endif
