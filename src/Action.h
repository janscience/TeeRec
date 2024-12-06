/*
  Action - base class for configurable menu entries.
  Created by Jan Benda, November 7, 2023.
*/

#ifndef Action_h
#define Action_h


#include <Arduino.h>


class FsFile;
class Configurable;
class Configurator;


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
    AllRoles = FileIO | StreamIO
  };

  /* Ask a yes or no question on a serial I/O stream. */
  static bool yesno(const char *request, bool defval=true,
		    bool echo=true, Stream &stream=Serial);

  /* Initialize action with name and supported roles. */
  Action(const char *name, int roles=AllRoles);

  /* Initialize action with name and supported roles and add it to menu. */
  Action(Configurable &menu, const char *name, int roles=AllRoles);

  /* The name identifying the action. */
  const char *name() const { return Name; }

  /* Set the name identifying the action to name. */
  void setName(const char *name);

  /* The parent menu of this action. */
  Configurable *parent() const { return Parent; };

  /* Set the parent of this action. */
  void setParent(Configurable *parent) { Parent = parent; };

  /* The root menu of this action. */
  Configurator *root();

  /* Return this Action if name matches its name. */
  virtual Action *action(const char *name);

  /* True if the specified roles are enabled. */
  bool enabled(int roles=AllRoles) const;

  /* True if the specified roles are not enabled. */
  bool disabled(int roles=AllRoles) const;

  /* Enable the specified roles for this action, if supported. */
  void enable(int roles=AllRoles);

  /* Disable the specified roles, if supported. */
  void disable(int roles=AllRoles);

  /* Disable the specified roles from the supported roles. */
  void disableSupported(int roles);

  /* Return the roles this action currently has. */
  int roles() const { return Roles; };
  
  /* Return the roles this action in general supports. */
  int supportedRoles() const { return Roles; };

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
  virtual void save(FsFile &file, size_t indent=0, size_t w=0) const {};
  
  /* Interactive configuration of this action via serial stream.
     StreamInput and StreamOutput must be enabled.
     Returns from initial menu after timeout milliseconds.
     If echo, print out received input.
     If detailed provide additional infos for GUI applications. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);

  /* Parse the string val and configure the action accordingly.
     SetValue must be enabled. If StreamOutput is enabled,
     report the new value together with name on stream. */
  virtual void set(const char *val, const char *name=0,
		   Stream &stream=Serial) {};

  
 protected:

  static const size_t MaxName = 64;
  char Name[MaxName];

  int SupportedRoles;
  int Roles;

  size_t Indentation;

  Configurable *Parent;
  
};


#endif
