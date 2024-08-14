/*
  Configurator - configure configurable class instances from a configuration file.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Configurator_h
#define Configurator_h


#include <Configurable.h>


class Configurator : public Configurable {

 public:

  /* Initialize with default name "Menu". */
  Configurator();

  /* Initialize. */
  Configurator(const char *name);

  /* Name of the configuration file. */
  const char *configFile() const { return ConfigFile; };

  /* Set name of the configuration file. */
  void setConfigFile(const char *fname);

  /* Report configuration menu on stream. */
  virtual void report(Stream &stream=Serial, size_t indent=0,
		      size_t w=0, bool descend=true) const;
  
  /* Save current setting to configuration file on SD card.
     Return true on success. */
  bool save(SDCard &sd) const;

  /* Read configuration file from SD card and pass key-value pairs to
     the Actions. */
  void load(SDCard &sd);

  /* Interactive configuration via Serial stream. */
  void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* The main menu, i.e. pointer to the last Configurator instance. */
  static Configurator *MainConfig;

  /* The submenu where Configurables are automatically added to. */
  Configurable *Config;

  
 private:

  static const size_t MaxFile = 128;
  char ConfigFile[MaxFile];

};


#endif
