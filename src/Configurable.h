/*
  Configurable - base class for all configurable classes.
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Configurable_h
#define Configurable_h


#include <Parameter.h>
#include <SDWriter.h>


class Configurable {

 public:

  /* Initialize configuration section name. */
  Configurable(const char *name);

  /* All lower case name of the configurable. */
  const char *name() const { return ConfigName; };

  /* Set name of the configurable to name. */
  void setName(const char *name);

  /* Add a parameter to this Configurable. */
  void add(Parameter *param);

  /* Return the Parameter matching key. */
  Parameter *parameter(const char *key);

  /* Enable the parameter matching key. */
  void enable(const char *key);

  /* Disable the parameter matching key, i.e. it will not be configured
     or written into a configuration file. */
  void disable(const char *key);

  /* Report configuration settings on Serial. */
  virtual void report() const;

  /* Configure the class with the provided key-value pair. */
  virtual void configure(const char *key, const char *val);

  /* Save current settings to file. */
  virtual void save(File &file) const;

  /* True if this was configured from a file, for example. */
  bool configured() const { return Configured; };

  /* Called from Configurator when this configurable is configured. */
  void setConfigured() { Configured = true; };


protected:

  static const size_t MaxName = 63;
  char ConfigName[MaxName+1];
  bool Configured;
  static const size_t MaxParams = 32;
  size_t NParams;
  Parameter *Params[MaxParams];

};


#endif
