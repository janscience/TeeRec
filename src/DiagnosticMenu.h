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


class TeensyInfoAction : public InfoAction {

 public:

  /* Initialize and add to configuration menu. */
  TeensyInfoAction(Menu &menu, const char *name);
  
  /* Update CPU speed. */
  void update();

 protected:

  char EEPROMLength[16];
};


class EEPROMHexdumpAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  EEPROMHexdumpAction(Menu &menu, const char *name);

  /* Hexdump of EEPROM memory. */
  virtual void execute(Stream &stream=Serial);
};


class EEPROMClearAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  EEPROMClearAction(Menu &menu, const char *name);
  
  /* Write 0xFF to all EEPROM memory. */
  virtual void execute(Stream &stream=Serial);
};


class PSRAMInfoAction : public InfoAction {

 public:

  /* Initialize and add to configuration menu. */
  PSRAMInfoAction(Menu &menu, const char *name);

 protected:
  
  char SizeStr[8];
  char CCMStr[24];
};


class PSRAMTestAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  PSRAMTestAction(Menu &menu, const char *name);

  /* Test Teensy 4.1 PSRAM memory. */
  virtual void execute(Stream &stream=Serial);

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
  virtual void write(Stream &stream=Serial, unsigned int roles=AllRoles,
		     size_t indent=0, size_t width=0, bool descend=true) const;

  /* Print more detailed device info. */
  virtual void execute(Stream &stream=Serial);

  /* Return device at index. Return zero for invalid index. */
  Device *device(size_t index);

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
  virtual void write(Stream &stream=Serial, unsigned int roles=AllRoles,
		     size_t indent=0, size_t width=0, bool descend=true) const;

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

  /* Update CPU speed. */
  void updateCPUSpeed();

  TeensyInfoAction TeensyInfoAct;
  EEPROMHexdumpAction EEPROMHexdumpAct;
  EEPROMClearAction EEPROMClearAct;
  PSRAMInfoAction PSRAMInfoAct;
  PSRAMTestAction PSRAMTestAct;
  SDCheckAction SD0CheckAct;
  SDBenchmarkAction SD0BenchmarkAct;
  SDCheckAction SD1CheckAct;
  SDBenchmarkAction SD1BenchmarkAct;
  DevicesAction DevicesAct;
  DeviceIDAction DeviceIDAct;

protected:

  void setSDCardNames(SDCard &sdcard, Action &checkact,
		      Action &benchmarkact);
};


#endif
