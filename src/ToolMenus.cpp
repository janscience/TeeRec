#include <ToolMenus.h>


DateTimeMenu::DateTimeMenu(RTClock &rtclock) :
  Configurable("Date & time", Action::StreamInput),
  ReportAct(*this, "Print date & time", rtclock),
  SetAct(*this, "Set date & time", rtclock) {
}


ConfigurationMenu::ConfigurationMenu(SDCard &sdcard) :
  Configurable("Configuration", Action::StreamInput),
  ReportAct(*this, "Print configuration"),
  SaveAct(*this,"Save configuration", sdcard),
  LoadAct(*this, "Load configuration", sdcard),
  RemoveAct(*this, "Erase configuration", sdcard) {
}


SDCardMenu::SDCardMenu(const char *name, SDCard &sdcard, Settings &settings) :
  Configurable(name, Action::StreamInput),
  ListRootAct(*this, "List files in root directory", sdcard),
  ListRecsAct(*this, "List all recordings", sdcard, settings),
  EraseRecsAct(*this, "Erase all recordings", sdcard, settings),
  FormatAct(*this, "Format SD card", sdcard),
  EraseFormatAct(*this, "Erase and format SD card", sdcard) {
}


#ifdef FIRMWARE_UPDATE
FirmwareMenu::FirmwareMenu(SDCard &sdcard) :
  Configurable("Firmware", Action::StreamInput),
  ListAct(*this, "List available updates", sdcard),
  UpdateAct(*this, "Update firmware", sdcard) {
}
#endif


DiagnosticMenu::DiagnosticMenu(const char *name, SDCard &sdcard) :
  Configurable(name, Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0InfoAct(*this, "SD card info", sdcard),
  SD0CheckAct(*this, "SD card check", sdcard),
  SD0BenchmarkAct(*this, "SD card benchmark", sdcard),
  SD1InfoAct(*this, "SD card info", sdcard),
  SD1CheckAct(*this, "SD card check", sdcard),
  SD1BenchmarkAct(*this, "SD card benchmark", sdcard) {
  SD1InfoAct.disable();
  SD1CheckAct.disable();
  SD1BenchmarkAct.disable();
}


DiagnosticMenu::DiagnosticMenu(const char *name, SDCard &sdcard0,
			       SDCard &sdcard1) :
  Configurable(name, Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0InfoAct(*this, "Primary SD card info", sdcard0),
  SD0CheckAct(*this, "Primary SD card check", sdcard0),
  SD0BenchmarkAct(*this, "Primary SD card benchmark", sdcard0),
  SD1InfoAct(*this, "Secondary SD card info", sdcard1),
  SD1CheckAct(*this, "Secondary SD card check", sdcard1),
  SD1BenchmarkAct(*this, "Secondary SD card benchmark", sdcard1) {
}


