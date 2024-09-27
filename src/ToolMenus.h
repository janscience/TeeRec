/*
  ToolMenus - Menus with actions for managing configurations, SD cards, and the real-time clock.
  Created by Jan Benda, September 27th, 2024.
*/

#ifndef ToolMenus_h
#define ToolMenus_h


#include <Configurable.h>
#include <ToolActions.h>


class DateTimeMenu : public Configurable {

public:

  DateTimeMenu(RTClock &rtclock);

protected:

  ReportRTCAction ReportAct;
  SetRTCAction SetAct;
  
};


class ConfigurationMenu : public Configurable {

public:

  ConfigurationMenu(SDCard &sdcard);

protected:

  ReportConfigAction ReportAct;
  SaveConfigAction SaveAct;
  LoadConfigAction LoadAct;
  RemoveConfigAction RemoveAct;
  
};


class SDCardMenu : public Configurable {

public:

  SDCardMenu(const char *name, SDCard &sdcard, Settings &settings);

protected:

  SDInfoAction InfoAct;
  SDCheckAction CheckAct;
  SDBenchmarkAction BenchmarkAct;
  SDFormatAction FormatAct;
  SDEraseFormatAction EraseFormatAct;
  SDListRootAction ListRootAct;
  SDListRecordingsAction ListRecsAct;
  SDRemoveRecordingsAction EraseRecsAct;
  
};


#ifdef FIRMWARE_UPDATE
class FirmwareMenu : public Configurable {

public:

  FirmwareMenu(SDCard &sdcard);

protected:

  ListFirmwareAction ListAct;
  UpdateFirmwareAction UpdateAct;
  
};
#endif


#endif
