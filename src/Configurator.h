/*
  Configurator - configure configurable class instances from a configuration file.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Configurator_h
#define Configurator_h


#include <Configurable.h>


class SDCard;


class Configurator : public Configurable {

 public:

  /* Initialize with default name "Configure". */
  Configurator();

  /* Initialize. */
  Configurator(const char *name);

  /* Name of the configuration file. */
  void setConfigFile(const char *fname);
  
  /* Save current setting to configuration file on SD card.
     Return true on success. */
  bool save(SDCard &sd) const;

  /* Interactive configuration via Serial stream. */
  void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Read configuration file from SD card and pass key-value pairs to
     the Actions. */
  void configure(SDCard &sd);

  static Configurator *Config;

  
 private:

  static const size_t MaxFile = 128;
  char ConfigFile[MaxFile];

};



#endif
