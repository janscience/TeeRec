/*
  ToolMenus - Menus with actions for managing configurations, SD cards, and the real-time clock.
  Created by Jan Benda, September 27th, 2024.
*/

#ifndef ToolMenus_h
#define ToolMenus_h


#include <ToolActions.h>


class DateTimeMenu : public Menu {

public:

  DateTimeMenu(RTClock &rtclock);

protected:

  PrintRTCAction PrintAct;
  ReportRTCAction ReportAct;
  SetRTCAction SetAct;
  
};


class SDCardMenu : public Menu {

public:

  SDCardMenu(SDCard &sdcard, Settings &settings);

protected:

  SDInfoAction InfoAct;
  SDListRootAction ListRootAct;
  SDListRecordingsAction ListRecsAct;
  SDRemoveRecordingsAction EraseRecsAct;
  SDFormatAction FormatAct;
  SDEraseFormatAction EraseFormatAct;
  
};


class InputMenu : public Menu {

public:

  InputMenu(Input &data, InputSettings &settings,
	    Device** controls=0, size_t ncontrols=0,
	    SetupAI setupai=0);

protected:

  ReportInputAction ReportAct;
  PrintInputAction PrintAct;
  
};


class DiagnosticMenu : public Menu {

public:

  DiagnosticMenu(const char *name, SDCard &sdcard, Device* dev0=0,
		 Device* dev1=0, Device* dev2=0, Device* dev3=0,
		 Device* dev4=0, Device* dev5=0);
  DiagnosticMenu(const char *name, SDCard &sdcard0, SDCard &sdcard1,
		 Device* dev0=0, Device* dev1=0,
		 Device* dev2=0, Device* dev3=0,
		 Device* dev4=0, Device* dev5=0);

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
};


#endif
