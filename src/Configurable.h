/*
  Configurable - A collection of Actions
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Configurable_h
#define Configurable_h


#include <Action.h>


class SDCard;
class FsFile;


class Configurable : public Action {

 public:

  /* Initialize configuration section name and add it to default menu. */
  Configurable(const char *name, int roles=AllRoles);

  /* Initialize configuration section name and add it to menu. */
  Configurable(Configurable &menu, const char *name, int roles=AllRoles);

  /* Add an action to this Configurable. */
  void add(Action *action);

  /* Return the Action matching name. */
  virtual Action *action(const char *name);

  /* Enable the roles of the action matching name. */
  void enable(const char *name, int roles=AllRoles);

  /* Disable the roles of the action matching name. */
  void disable(const char *name, int roles=AllRoles);

  /* Report name on stream. If descend, also display name and values
     of children. */
  virtual void report(Stream &stream=Serial, size_t indent=0,
		      size_t w=0, bool descend=true) const;

  /* Save current settings to file. */
  virtual void save(FsFile &file, size_t indent=0, size_t w=0) const;

  /* Save current setting to configuration file on SD card.
     Return true on success. */
  bool save(SDCard &sd, const char *filename) const;

  /* Read configuration file from SD card and configure all actions
     accordingly. */
  void load(SDCard &sd, const char *filename);
  
  /* Interactive configuration via Serial stream.
     Returns from initial menu after timeout milliseconds. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);

  /* Set the provided name-value pair and report on stream. */
  virtual void set(const char *val, const char *name,
		   Stream &stream=Serial);


protected:

  static const size_t MaxActions = 32;
  size_t NActions;
  Action *Actions[MaxActions];

};


#endif
