/*
  Configurator - configure configurable class instances from a configuration file.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Configurator_h
#define Configurator_h


#include <Configurable.h>


class SDCard;
class Configurator;


class ConfigureAction : public Configurable {

 public:

  /* Initialize and add to default configurator. */
  ConfigureAction(const char *name);
  
  /* Initialize and add to config. */
  ConfigureAction(const char *name, Configurable &config);
  
};


class ReportAction : public Action {

 public:

  /* Initialize. */
  ReportAction(const char *name);

  /* Initialize. */
  ReportAction(const char *name, Configurable &config);

  /* Report the configuration settings. */
  virtual void execute();


 private:

  Configurable *Config;
  
};


class SaveAction : public Action {

 public:

  /* Initialize. */
  SaveAction(const char *name, SDCard &sd, Configurator &config);

  /* Initialize. */
  SaveAction(const char *name, SDCard &sd, Configurator &config,
	     Configurable &menu);

  /* Save the configuration settings. */
  virtual void execute();


 private:

  Configurator *Config;
  SDCard *SDC;
  
};


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

  /* Report name on stream. If descend, also display name and values
     of children. */
  virtual void report(Stream &stream=Serial, size_t indent=0,
		      size_t w=0, bool descend=true) const;
  
  /* Save current setting to configuration file on SD card.
     Return true on success. */
  bool save(SDCard &sd) const;

  /* Interactive configuration via Serial stream. */
  void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Read configuration file from SD card and pass key-value pairs to
     the Actions. */
  void configure(SDCard &sd);

  static Configurable *Config;

  
 private:

  static const size_t MaxFile = 128;
  char ConfigFile[MaxFile];

};


#endif
