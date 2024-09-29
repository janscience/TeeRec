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

  SDListRootAction ListRootAct;
  SDListRecordingsAction ListRecsAct;
  SDRemoveRecordingsAction EraseRecsAct;
  SDFormatAction FormatAct;
  SDEraseFormatAction EraseFormatAct;
  
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


class DiagnosticMenu : public Configurable {

public:

  DiagnosticMenu(const char *name, SDCard &sdcard);
  DiagnosticMenu(const char *name, SDCard &sdcard0, SDCard &sdcard1);

protected:

  TeensyInfoAction TeensyInfoAct;
  PSRAMInfoAction PSRAMInfoAct;
  PSRAMTestAction PSRAMTestAct;
  SDInfoAction SD0InfoAct;
  SDCheckAction SD0CheckAct;
  SDBenchmarkAction SD0BenchmarkAct;
  SDInfoAction SD1InfoAct;
  SDCheckAction SD1CheckAct;
  SDBenchmarkAction SD1BenchmarkAct;
  
};


#endif
