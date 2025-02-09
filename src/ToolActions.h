/*
  ToolActions - Actions for device infos and checks, SD cards, and the real-time clock.
  Created by Jan Benda, August 13th, 2024.
*/

#ifndef ToolActions_h
#define ToolActions_h


#include <MicroConfig.h>


class SDCard;
class Settings;
class Device;
class Input;
class InputSettings;


class TeensyInfoAction : public Action {

 public:

  /* Initialize and add to default menu. */
  TeensyInfoAction(const char *name);

  /* Initialize and add to configuration menu. */
  TeensyInfoAction(Menu &menu, const char *name);

  /* Info about Teensy version, serial number and MAC. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class PSRAMInfoAction : public TeensyInfoAction {

 public:

  using TeensyInfoAction::TeensyInfoAction;

  /* Info about Teensy 4.1 PSRAM memory. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class PSRAMTestAction : public TeensyInfoAction {

 public:

  using TeensyInfoAction::TeensyInfoAction;

  /* Test Teensy 4.1 PSRAM memory. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);

 protected:

  bool checkFixed(uint32_t pattern, Stream &stream);
  bool checkRandom(uint32_t seed, Stream &stream);

  uint32_t *MemoryBegin;
  uint32_t *MemoryEnd;

};


class DevicesAction : public Action {

 public:

  /* Initialize and add to default menu. */
  DevicesAction(const char *name, Device* dev0, Device* dev1=0,
		Device* dev2=0, Device* dev3=0,
		Device* dev4=0, Device* dev5=0);

  /* Initialize and add to configuration menu. */
  DevicesAction(Menu &menu, const char *name,
		Device* dev0, Device* dev1=0,
		Device* dev2=0, Device* dev3=0,
		Device* dev4=0, Device* dev5=0);

  /* Print device infos. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);

protected:

  static const size_t MaxDevices = 6;
  size_t NDevices;
  Device *Devices[MaxDevices];
  
};


typedef void (*SetupAI)(Input &aidata, const InputSettings &aisettings,
		 Device **controls, size_t ncontrols, Stream &stream);

class InputAction : public Action {

 public:

  /* Initialize and add to default menu. */
  InputAction(const char *name, Input &data, InputSettings &settings,
	      Device** controls=0, size_t ncontrols=0, SetupAI setupai=0);

  /* Initialize and add to configuration menu. */
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


#endif
