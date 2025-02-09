#include <SDCard.h>
#include <ToolMenus.h>


DateTimeMenu::DateTimeMenu(RTClock &rtclock) :
  Menu("Date & time", Action::StreamInput),
  PrintAct(*this, "Print date & time", rtclock),
  ReportAct(*this, "Report date & time infos", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}


ConfigurationMenu::ConfigurationMenu(SDCard &sdcard) :
  Menu("Configuration", Action::StreamInput),
  ReportAct(*this, "Print configuration"),
  SaveAct(*this,"Save configuration", sdcard),
  LoadAct(*this, "Load configuration", sdcard),
  RemoveAct(*this, "Erase configuration", sdcard) {
}


SDCardMenu::SDCardMenu(SDCard &sdcard, Settings &settings) :
  Menu("SD", Action::StreamInput),
  InfoAct(*this, "SD card info", sdcard),
  ListRootAct(*this, "List files in root directory", sdcard),
  ListRecsAct(*this, "List all recordings", sdcard, settings),
  EraseRecsAct(*this, "Erase all recordings", sdcard, settings),
  FormatAct(*this, "Format SD card", sdcard),
  EraseFormatAct(*this, "Erase and format SD card", sdcard) {
  char name[64];
  strcpy(name, sdcard.name());
  if (strlen(name) > 0)
    name[0] = toupper(name[0]);
  strcat(name, "SD card");
  setName(name);
}


FirmwareMenu::FirmwareMenu(SDCard &sdcard) :
  Menu("Firmware", Action::StreamInput),
  ListAct(*this, "List available updates", sdcard),
  UpdateAct(*this, "Update firmware", sdcard) {
}


InputMenu::InputMenu(Input &data, InputSettings &settings,
		     Device** controls, size_t ncontrols, SetupAI setupai) :
  Menu("Analog input", Action::StreamInput),
  ReportAct(*this, "Report input configuration", data, settings,
	    controls, ncontrols, setupai),
  PrintAct(*this, "Record some data", data, settings,
	   controls, ncontrols, setupai) {
}


DiagnosticMenu::DiagnosticMenu(const char *name, SDCard &sdcard,
			       Device* dev0, Device* dev1,
			       Device* dev2, Device* dev3,
			       Device* dev4, Device* dev5) :
  Menu(name, Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0CheckAct(*this, "SDc", sdcard),
  SD0BenchmarkAct(*this, "SDb", sdcard),
  SD1CheckAct(*this, "SDc", sdcard),
  SD1BenchmarkAct(*this, "SDb", sdcard),
  DevicesAct(*this, "Input devices", dev0, dev1, dev2, dev3, dev4, dev5) {
  setSDCardNames(sdcard, SD0CheckAct, SD0BenchmarkAct);
  SD1CheckAct.disable();
  SD1BenchmarkAct.disable();
}


DiagnosticMenu::DiagnosticMenu(const char *name, SDCard &sdcard0,
			       SDCard &sdcard1,
			       Device* dev0, Device* dev1,
			       Device* dev2, Device* dev3,
			       Device* dev4, Device* dev5) :
  Menu(name, Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0CheckAct(*this, "Primary SD card check", sdcard0),
  SD0BenchmarkAct(*this, "Primary SD card benchmark", sdcard0),
  SD1CheckAct(*this, "Secondary SD card check", sdcard1),
  SD1BenchmarkAct(*this, "Secondary SD card benchmark", sdcard1),
  DevicesAct(*this, "Input devices", dev0, dev1, dev2, dev3, dev4, dev5) {
  setSDCardNames(sdcard0, SD0CheckAct, SD0BenchmarkAct);
  setSDCardNames(sdcard1, SD1CheckAct, SD1BenchmarkAct);
}


void DiagnosticMenu::setSDCardNames(SDCard &sdcard, Action &checkact,
				    Action &benchmarkact) {
  char name[64];
  strcpy(name, sdcard.name());
  if (strlen(name) > 0)
    name[0] = toupper(name[0]);
  strcat(name, "SD card check");
  checkact.setName(name);
  name[strlen(name) - 5] = '\0';
  strcat(name, "benchmark");
  benchmarkact.setName(name);
}
