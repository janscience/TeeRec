/*
  Configurable - base class for all configurable classes.
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Configurable_h
#define Configurable_h


#include <Action.h>


class File;


class Configurable {

 public:

  /* Initialize configuration section name. */
  Configurable(const char *name);

  /* All lower case name of the configurable. */
  const char *name() const { return ConfigName; };

  /* Set name of the configurable to name. */
  void setName(const char *name);

  /* Add an action to this Configurable. */
  void add(Action *action);

  /* Return the Action matching name. */
  Action *action(const char *name);

  /* Enable the parameter matching name. */
  void enable(const char *name);

  /* Disable the parameter matching name, i.e. it will not be configured
     or written into a configuration file. */
  void disable(const char *name);
  
  /* Interactive configuration via Serial stream. */
  void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Configure the class with the provided name-value pair. */
  void configure(const char *name, const char *val);

  /* Report configuration settings on Serial. */
  void report(size_t indent=0) const;

  /* Save current settings to file. */
  void save(File &file, size_t indent=0) const;

  /* True if this was configured from a file, for example. */
  bool configured() const { return Configured; };

  /* Called from Configurator when this configurable is configured. */
  void setConfigured() { Configured = true; };


protected:

  static const size_t MaxName = 64;
  char ConfigName[MaxName];
  bool Configured;
  static const size_t MaxActions = 32;
  size_t NActions;
  Action *Actions[MaxActions];

};


#endif
