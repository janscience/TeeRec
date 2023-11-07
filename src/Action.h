/*
  Action - base class for configurable menu entries.
  Created by Jan Benda, November 7, 2023.
*/

#ifndef Action_h
#define Action_h


#include <Arduino.h>


class File;
class Configurable;


class Action {

 public:

  /* Initialize action with name. */
  Action(const char *name);

  /* Initialize action with name and add it to cfg. */
  Action(Configurable *cfg, const char *name);

  /* The name identifying the action. */
  const char *name() const { return Name; }

  /* Set the name identifying the action to name. */
  void setName(const char *name);

  /* True if this action is enabled. */
  bool enabled() const { return Enabled; };

  /* True if this action is disabled. */
  bool disabled() const { return !Enabled; };

  /* Make this action configurable (default). */
  void enable();

  /* Make this action non-configurable. */
  void disable();

  /* Report the action's name and potential values or infos on Serial.
     If descend, also display children. */
  virtual void report(size_t indent=0, size_t w=0, bool descend=true) const;

  /* Save the actions's name and potential value to file. */
  virtual void save(File &file, size_t indent=0, size_t w=0) const {};
  
  /* Interactive configuration of this action via Serial stream. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0) {};

  /* Parse the string val, configure the action accordingly and report
     result together with name on Serial. */
  virtual void configure(const char *val, const char *name=0) {};

  
 protected:

  static const size_t MaxName = 64;
  char Name[MaxName];

  bool Enabled;
  
};


#endif
