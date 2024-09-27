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
  InfoAct(*this, "SD card info", sdcard),
  CheckAct(*this, "SD card check", sdcard),
  BenchmarkAct(*this, "SD card benchmark", sdcard),
  FormatAct(*this, "Format SD card", sdcard),
  EraseFormatAct(*this, "Erase and format SD card", sdcard),
  ListRootAct(*this, "List files in root directory", sdcard),
  ListRecsAct(*this, "List all recordings", sdcard, settings),
  EraseRecsAct(*this, "Erase all recordings", sdcard, settings) {
}


#ifdef FIRMWARE_UPDATE
FirmwareMenu::FirmwareMenu(SDCard &sdcard) :
  Configurable("Firmware", Action::StreamInput),
  ListAct(*this, "List available updates", sdcard),
  UpdateAct(*this, "Update firmware", sdcard) {
}
#endif

