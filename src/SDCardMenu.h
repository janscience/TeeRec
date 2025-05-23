/*
  SDCardMenu - Actions and menu for dealing with SD cards.
  Created by Jan Benda, February 9th, 2025.
*/

#ifndef SDCardMenu_h
#define SDCardMenu_h


#include <MicroConfig.h>


class SDCard;
class Settings;


class SDCardAction : public Action {

 public:

  /* Initialize and add to configuration menu. */
  SDCardAction(Menu &menu, const char *name, SDCard &sd);

 protected:

  SDCard &SDC; 
};


class SDInfoAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Report SD card infos, capacity and available space. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDCheckAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Check SD card access. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDBenchmarkAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Run a benchmark test and report data rates for writing and reading. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDFormatAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* Format SD card. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);

 protected:

  void format(const char *erases, bool erase, bool echo, Stream &stream);
};


class SDEraseFormatAction : public SDFormatAction {

 public:

  using SDFormatAction::SDFormatAction;

  /* Erase and format SD card. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDListRootAction : public SDCardAction {

 public:

  using SDCardAction::SDCardAction;

  /* List files and directories of the root directory. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDListRecordingsAction : public SDCardAction {

 public:

  /* Initialize and add to configuration menu. */
  SDListRecordingsAction(Menu &menu, const char *name, SDCard &sd,
			 Settings &settings);

  /* List all recordings on SD card. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);

 protected:
  
  Settings &SettingsMenu;
};


class SDRemoveRecordingsAction : public SDListRecordingsAction {

 public:

  using SDListRecordingsAction::SDListRecordingsAction;

  /* Remove all recordings from SD card. */
  virtual void execute(Stream &stream=Serial, unsigned long timeout=0,
		       bool echo=true, bool detailed=false);
};


class SDCardMenu : public Menu {

public:

  SDCardMenu(Menu &menu, SDCard &sdcard, Settings &settings);

protected:

  SDInfoAction InfoAct;
  SDListRootAction ListRootAct;
  SDListRecordingsAction ListRecsAct;
  SDRemoveRecordingsAction EraseRecsAct;
  SDFormatAction FormatAct;
  SDEraseFormatAction EraseFormatAct;
  
};


#endif
