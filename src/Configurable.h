/*
  Configurable - base class for all configurable classes.
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Configurable_h
#define Configurable_h


#include <Action.h>


class File;


class Configurable : public Action {

 public:

  /* Initialize configuration section name. */
  Configurable(const char *name);

  /* Add an action to this Configurable. */
  void add(Action *action);

  /* Return the Action matching name. */
  Action *action(const char *name);

  /* Enable the action matching name. */
  void enable(const char *name);

  /* Disable the action matching name, i.e. it will not be configured
     or written into a configuration file. */
  void disable(const char *name);

  /* Report name on Serial. If descend, also display name and values
     of children. */
  virtual void report(size_t indent=0, size_t w=0, bool descend=true) const;

  /* Save current settings to file. */
  virtual void save(File &file, size_t indent=0, size_t w=0) const;
  
  /* Interactive configuration via Serial stream. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Configure the class with the provided name-value pair. */
  virtual void configure(const char *name, const char *val);

  /* True if this was configured from a file, for example. */
  bool configured() const { return Configured; };

  /* Called from Configurator when this configurable is configured. */
  void setConfigured() { Configured = true; };


protected:

  bool Configured;
  static const size_t MaxActions = 32;
  size_t NActions;
  Action *Actions[MaxActions];

};


#endif
