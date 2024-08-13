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
  
  /* Initialize and add to configuration menu. */
  ConfigureAction(const char *name, Configurable &menu);
  
};


class ReportConfigAction : public Action {

 public:

  /* Initialize. */
  ReportConfigAction(const char *name);

  /* Initialize and add to configuration menu. */
  ReportConfigAction(const char *name, Configurable &menu);

  /* Report the configuration settings. */
  virtual void execute();
  
};


class SDCardAction : public Action {

 public:

  /* Initialize. */
  SDCardAction(const char *name, SDCard &sd);

 protected:

  SDCard &SDC; 
};


class SaveConfigAction : public SDCardAction {

 public:

  /* Initialize. */
  SaveConfigAction(const char *name, SDCard &sd);

  /* Initialize. */
  SaveConfigAction(const char *name, SDCard &sd, Configurable &menu);

  /* Save the configuration settings to configuration file. */
  virtual void execute();
};


class LoadConfigAction : public SDCardAction {

 public:

  /* Initialize. */
  LoadConfigAction(const char *name, SDCard &sd);

  /* Initialize. */
  LoadConfigAction(const char *name, SDCard &sd, Configurable &menu);

  /* Load the configuration settings from configuration file. */
  virtual void execute();
};


class RemoveConfigAction : public SDCardAction {

 public:

  /* Initialize. */
  RemoveConfigAction(const char *name, SDCard &sd);

  /* Initialize. */
  RemoveConfigAction(const char *name, SDCard &sd, Configurable &menu);

  /* Remove the configuration file from SD card. */
  virtual void execute();
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

  /* Report configuration menu on stream. */
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

  /* The main menu, i.e. pointer to the last Configurator instance. */
  static Configurator *MainConfig;

  /* The menu where Configurables are automatically added to. */
  static Configurable *Config;

  
 private:

  static const size_t MaxFile = 128;
  char ConfigFile[MaxFile];

};


#endif
