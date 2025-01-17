/*
  ToolActions - Actions for managing configurations, SD cards, and the real-time clock.
  Created by Jan Benda, August 13th, 2024.
*/

#ifndef ToolActions_h
#define ToolActions_h


#include <Action.h>
#include <Configurable.h>
#include <FirmwareUpdate.h>


class SDCard;
class Settings;
class RTClock;
class Device;


class HelpAction : public Action {

 public:

  /* Initialize and add to default menu. */
  HelpAction(const char *name);

  /* Initialize and add to configuration menu. */
  HelpAction(Configurable &menu, const char *name);

  /* Print help and key-bindings. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class TeensyInfoAction : public Action {

 public:

  /* Initialize and add to default menu. */
  TeensyInfoAction(const char *name);

  /* Initialize and add to configuration menu. */
  TeensyInfoAction(Configurable &menu, const char *name);

  /* Info about Teensy version, serial number and MAC. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class PSRAMInfoAction : public TeensyInfoAction {

 public:

  using TeensyInfoAction::TeensyInfoAction;

  /* Info about Teensy 4.1 PSRAM memory. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class PSRAMTestAction : public TeensyInfoAction {

 public:

  using TeensyInfoAction::TeensyInfoAction;

  /* Test Teensy 4.1 PSRAM memory. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);

 protected:

  bool checkFixed(uint32_t pattern, Stream &stream);
  bool checkRandom(uint32_t seed, Stream &stream);

  uint32_t *MemoryBegin;
  uint32_t *MemoryEnd;

};


class ReportConfigAction : public Action {

 public:

  /* Initialize and add to default menu. */
  ReportConfigAction(const char *name);

  /* Initialize and add to configuration menu. */
  ReportConfigAction(Configurable &menu, const char *name);

  /* Report the configuration settings. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDCardAction : public Action {

 public:

  /* Initialize and add to default menu. */
  SDCardAction(const char *name, SDCard &sd);

  /* Initialize and add to configuration menu. */
  SDCardAction(Configurable &menu, const char *name, SDCard &sd);

 protected:

  SDCard &SDC; 
};


class SaveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Save the configuration settings to configuration file. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class LoadConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Load the configuration settings from configuration file. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class RemoveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Remove the configuration file from SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDInfoAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Report SD card infos, capacity and available space. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDCheckAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Check SD card access. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDBenchmarkAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Run a benchmark test and report data rates for writing and reading. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDFormatAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Format SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);

 protected:

  void format(const char *erases, bool erase, bool echo, Stream &stream);
};


class SDEraseFormatAction : public SDFormatAction {

 public:

  using SDFormatAction::SDFormatAction;

  /* Erase and format SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDListRootAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* List files and directories of the root directory. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SDListRecordingsAction : public SDCardAction {

 public:

  /* Initialize and add to default menu. */
  SDListRecordingsAction(const char *name, SDCard &sd, Settings &settings);

  /* Initialize and add to configuration menu. */
  SDListRecordingsAction(Configurable &menu, const char *name, SDCard &sd,
		 Settings &settings);

  /* List all recordings on SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);

 protected:
  
  Settings &SettingsMenu;
};


class SDRemoveRecordingsAction : public SDListRecordingsAction {

 public:

  using SDListRecordingsAction::SDListRecordingsAction;

  /* Remove all recordings from SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class ListFirmwareAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* List firmware hex files found on SD card */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class UpdateFirmwareAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Upload firmware from SD card */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class RTCAction : public Action {

 public:

  /* Initialize and add to default menu. */
  RTCAction(const char *name, RTClock &rtclock);

  /* Initialize and add to configuration menu. */
  RTCAction(Configurable &menu, const char *name, RTClock &rtclock);

 protected:

  RTClock &RTC;
};


class PrintRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Print the current time. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class ReportRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Report the time and the status of the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);
};


class DevicesAction : public Action {

 public:

  /* Initialize and add to default menu. */
  DevicesAction(const char *name, Device* dev0, Device* dev1=0,
		Device* dev2=0, Device* dev3=0,
		Device* dev4=0, Device* dev5=0);

  /* Initialize and add to configuration menu. */
  DevicesAction(Configurable &menu, const char *name,
		Device* dev0, Device* dev1=0,
		Device* dev2=0, Device* dev3=0,
		Device* dev4=0, Device* dev5=0);

  /* Print device infos. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0,
			 bool echo=true, bool detailed=false);

protected:

  static const size_t MaxDevices = 6;
  size_t NDevices;
  Device *Devices[MaxDevices];
  
};


#endif
