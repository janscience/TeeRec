/*
  Configurator - configure configurable class instances from a configuration file.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Configurator_h
#define Configurator_h


#include <Configurable.h>


class SDCard;
class Configurator;


class ReportAction : public Action {

 public:

  /* Initialize with name "Print configuration". */
  ReportAction(const Configurator *config);

  /* Report the configuration settings. */
  virtual void execute();


 private:

  const Configurator *Config;
  
};


class SaveAction : public Action {

 public:

  /* Initialize with name "Save configuration". */
  SaveAction(const Configurator *config);

  /* Save the configuration settings. */
  virtual void execute();


 private:

  const Configurator *Config;
  
};


class Configurator : public Configurable {

 public:

  /* Initialize with default name "Configure". */
  Configurator();

  /* Initialize. */
  Configurator(const char *name);

  /* Name of the configuration file. */
  const char *configFile() { return ConfigFile; };

  /* Set name of the configuration file. */
  void setConfigFile(const char *fname);

  /* Add Configurable for configuration and move all Actions into it. */
  void addConfigure();

  /* Add action that shows all configuration settings. */
  void addReport();

  /* Add action that saves all configuration settings to SD card. */
  void addSave();
  
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

  size_t MActions;
  
  Configurable ConfigureAct;
  ReportAction ReportAct;
  SaveAction SaveAct;

};


#endif
