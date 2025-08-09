/*
  DiagnosticMenu - Actions and menu for diagnostics of the Teensy board, PSRAM memory, and devices.
  Created by Jan Benda, August 13th, 2024.
*/

#ifndef DiagnosticMenu_h
#define DiagnosticMenu_h


#include <SDCardMenu.h>
#include <MicroConfig.h>


class Device;
class DeviceID;
class SDCard;


class TeensyInfoAction : public Action {

 public:

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


class DeviceIDAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  DeviceIDAction(Menu &menu, const char *name,
		 DeviceID *deviceid);

  /* Read and print device ID. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);

protected:

  DeviceID *DevID;
  
};


class DiagnosticMenu : public Menu {

public:

  DiagnosticMenu(Menu &menu, SDCard &sdcard, DeviceID *devid,
		 Device* dev0=0, Device* dev1=0, Device* dev2=0,
		 Device* dev3=0, Device* dev4=0, Device* dev5=0);
  DiagnosticMenu(Menu &menu, SDCard &sdcard0, SDCard &sdcard1,
		 DeviceID *devid,
		 Device* dev0=0, Device* dev1=0, Device* dev2=0,
		 Device* dev3=0, Device* dev4=0, Device* dev5=0);

protected:

  void setSDCardNames(SDCard &sdcard, Action &checkact,
		      Action &benchmarkact);

  TeensyInfoAction TeensyInfoAct;
  PSRAMInfoAction PSRAMInfoAct;
  PSRAMTestAction PSRAMTestAct;
  SDCheckAction SD0CheckAct;
  SDBenchmarkAction SD0BenchmarkAct;
  SDCheckAction SD1CheckAct;
  SDBenchmarkAction SD1BenchmarkAct;
  DevicesAction DevicesAct;
  DeviceIDAction DeviceIDAct;
};


#endif
