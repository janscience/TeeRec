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

  enum Role {
    SetValue = 1,    // set value from a string
    FileOutput = 2,  // write to configuration file
    FileInput = 4,   // read from configuration file
    FileIO = FileInput | FileOutput,
    StreamOutput = 8,
    StreamInput = 16,
    StreamIO = StreamInput | StreamOutput,
    // DisplayUpDownButtons,
    // DisplayTouch,
    // whatever input/output device
    AllRoles = SetValue | FileIO | StreamIO
  };

  /* Initialize action with name and supported roles. */
  Action(const char *name, int roles=AllRoles);

  /* Initialize action with name and supported roles and add it to cfg. */
  Action(Configurable *cfg, const char *name, int roles=AllRoles);

  /* The name identifying the action. */
  const char *name() const { return Name; }

  /* Set the name identifying the action to name. */
  void setName(const char *name);

  /* Return this Action if name matches it name. */
  virtual Action *action(const char *name);

  /* True if the specified roles are enabled. */
  bool enabled(int roles=AllRoles) const;

  /* True if the specified roles are not enabled. */
  bool disabled(int roles=AllRoles) const;

  /* Enable the specified roles for this action, if supported. */
  void enable(int roles=AllRoles);

  /* Disable the specified roles, if supported. */
  void disable(int roles=AllRoles);

  /* The number of spaces to be used for each indentation level. */
  size_t indentation() const { return Indentation; };

  /* Set the number of spaces to be used for each indentation level. */
  void setIndentation(size_t indentation) { Indentation = indentation; };

  /* Report the action's name and potential values or infos on stream.
     If descend, also display children.
     StreamOutput must be enabled. */
  virtual void report(Stream &stream=Serial, size_t indent=0,
		      size_t w=0, bool descend=true) const;

  /* Save the actions's name and potential value to file.
     FileOutput must be enabled. */
  virtual void save(File &file, size_t indent=0, size_t w=0) const {};
  
  /* Interactive configuration of this action via serial stream.
     StreamInput and StreamOutput must be enabled. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0) {};

  /* Parse the string val and configure the action accordingly.
     SetValue must be enabled. If StreamOutput is enabled,
     report the new value together with name on stream. */
  virtual void configure(const char *val, const char *name=0,
			 Stream &stream=Serial) {};

  
 protected:

  /* Disable the specified roles from the supported roles. */
  void disableSupported(int roles);

  static const size_t MaxName = 64;
  char Name[MaxName];

  int SupportedRoles;
  int Roles;

  size_t Indentation;
  
};


#endif
