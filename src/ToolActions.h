/*
  ToolActions - Actions for handling the configuration file and the real-time clock.
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


class ConfigureAction : public Configurable {

 public:

  /* Initialize and add to default menu. */
  ConfigureAction(const char *name, int roles=StreamInput);
  
  /* Initialize and add to configuration menu. */
  ConfigureAction(Configurable &menu, const char *name, int roles=StreamInput);
};


class ReportConfigAction : public Action {

 public:

  /* Initialize and add to default menu. */
  ReportConfigAction(const char *name);

  /* Initialize and add to configuration menu. */
  ReportConfigAction(Configurable &menu, const char *name);

  /* Report the configuration settings. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);  
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
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class LoadConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Load the configuration settings from configuration file. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class RemoveConfigAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Remove the configuration file from SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class SDInfoAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Report sectors and size of SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class SDFormatAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Format SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);

 protected:

  void format(const char *erases, bool erase, Stream &stream);
};


class SDEraseFormatAction : public SDFormatAction {

 public:

  using SDFormatAction::SDFormatAction;

  /* Erase and format SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


#ifdef FIRMWARE_UPDATE

class ListFirmwareAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* List firmware hex files found on SD card */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class UpdateFirmwareAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Upload firmware from SD card */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};

#endif


class SDListAction : public SDCardAction {

 public:

  /* Initialize and add to default menu. */
  SDListAction(const char *name, SDCard &sd, Settings &settings);

  /* Initialize and add to configuration menu. */
  SDListAction(Configurable &menu, const char *name, SDCard &sd,
		 Settings &settings);

  /* List all recordings on SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);

 protected:
  
  Settings &SettingsMenu;
};


class SDRemoveAction : public SDListAction {

 public:

  using SDListAction::SDListAction;

  /* Remove all recordings from SD card. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
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


class ReportRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Report the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


class SetRTCAction : public RTCAction {

 public:

  using RTCAction::RTCAction;

  /* Set the real-time clock. */
  virtual void configure(Stream &stream=Serial, unsigned long timeout=0);
};


#endif
