/*
  InputMenu - Actions and menu for checking and reading analog input data.
  Created by Jan Benda, February 9th, 2025.
*/

#ifndef InputMenu_h
#define InputMenu_h


#include <MicroConfig.h>


class Device;
class Input;
class InputSettings;


typedef void (*SetupAI)(Input &aidata, const InputSettings &aisettings,
		 Device **controls, size_t ncontrols, Stream &stream);

class InputAction : public Action {

 public:

  /* Initialize and add to menu. */
  InputAction(Menu &menu, const char *name,
	      Input &data, InputSettings &settings,
	      Device** controls=0, size_t ncontrols=0, SetupAI setupai=0);

 protected:

  Input &Data;
  InputSettings &Settings;
  Device **Controls;
  size_t NControls;
  SetupAI Setupai;
};


class ReportInputAction : public InputAction {

 public:

  using InputAction::InputAction;

  /* Start, report, and stop analog input. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class PrintInputAction : public InputAction {

 public:

  using InputAction::InputAction;

  /* Record and print 100ms of data. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class StartInputAction : public InputAction {

 public:

  using InputAction::InputAction;

  /* Start data acquisition with current settings. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class GetInputAction : public InputAction {

 public:

  using InputAction::InputAction;

  /* Print 100ms of data. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class StopInputAction : public InputAction {

 public:

  using InputAction::InputAction;

  /* Stop acquisition. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class InputMenu : public Menu {

public:

  InputMenu(Menu &menu, Input &data, InputSettings &settings,
	    Device** controls=0, size_t ncontrols=0,
	    SetupAI setupai=0);

  ReportInputAction ReportAct;
  PrintInputAction PrintAct;
  StartInputAction StartAct;
  GetInputAction GetAct;
  StopInputAction StopAct;
  
};


#endif
