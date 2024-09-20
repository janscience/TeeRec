#include <RTClock.h>
#include <SDWriter.h>
#include <Configurator.h>
#include <Settings.h>
#include <ToolActions.h>


ConfigureAction::ConfigureAction(const char *name, int roles) :
  ConfigureAction(*Configurator::MainConfig->Config, name, roles) {
}


ConfigureAction::ConfigureAction(Configurable &menu, const char *name,
				 int roles) :
  Configurable(menu, name, roles) {
  root()->Config = this;
}


ReportConfigAction::ReportConfigAction(const char *name) :
  ReportConfigAction(*Configurator::MainConfig->Config, name) {
}


ReportConfigAction::ReportConfigAction(Configurable &menu, const char *name) :
  Action(menu, name, StreamInput) {
}


void ReportConfigAction::configure(Stream &stream, unsigned long timeout) {
  root()->report(stream);
  stream.println();
}


SDCardAction::SDCardAction(const char *name, SDCard &sd) :
  SDCardAction(*Configurator::MainConfig->Config, name, sd) {
}


SDCardAction::SDCardAction(Configurable &menu, const char *name, SDCard &sd) : 
  Action(menu, name, StreamInput),
  SDC(sd) {
}


void SaveConfigAction::configure(Stream &stream, unsigned long timeout) {
  bool save = true;
  if (SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" already exists on SD card.\n",
		  root()->configFile());
    save = Action::yesno("Do you want to overwrite the configuration file?",
			 true, stream);
  }
  if (save && root()->save(SDC))
    stream.printf("Saved configuration to file \"%s\" on SD card.\n",
		  root()->configFile());
  stream.println();
}


void LoadConfigAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" not found on SD card.\n\n",
		  root()->configFile());
    return;
  }
  stream.println("Reloading the configuration file will discard all changes.");
  bool r = Action::yesno("Do you really want to reload the configuration file?",
			 true, stream);
  stream.println();
  if (r)
    root()->load(SDC);
}


void RemoveConfigAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" does not exist on SD card.\n\n",
		  root()->configFile());
    return;
  }
  if (Action::yesno("Do you really want to remove the configuration file?",
		    false, stream)) {
    if (SDC.remove(root()->configFile()))
      stream.printf("\nRemoved configuration file \"%s\" from SD card.\n\n",
		    root()->configFile());
    else
      stream.printf("\nERROR! Failed to remove configuration file \"%s\" from SD card.\n\n",
		    root()->configFile());
  }
  else
    stream.println();
}


void SDInfoAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  SDC.report(stream);
}


void SDBenchmarkAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  SDC.benchmark(512, 10, 2, stream);
}


void SDFormatAction::format(const char *erases, bool erase, Stream &stream) {
  char request[64];
  sprintf(request, "Do you really want to%s format the SD card?", erases);
  if (Action::yesno(request, false, stream)) {
    bool keep = false;
    if (SDC.exists(root()->configFile())) {
      char request[256];
      sprintf(request, "Should the configuration file \"%s\" be kept?",
	      root()->configFile());
      keep = Action::yesno(request, true, stream);
    }
    stream.println();
    const char *path = NULL;
    if (keep)
      path = root()->configFile();
    SDC.format(path, erase, stream);
  }
  else
    stream.println();
}


void SDFormatAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  stream.println("Formatting will destroy all data on the SD card.");
  format("", false, stream);
}


void SDEraseFormatAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  stream.println("Erasing and formatting will destroy all data on the SD card.");
  format(" erase and", true, stream);
}


#ifdef FIRMWARE_UPDATE

void ListFirmwareAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  listFirmware(SDC, stream);
  stream.println();
}


void UpdateFirmwareAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  updateFirmware(SDC, stream);
  stream.println();
}

#endif


SDListAction::SDListAction(const char *name, SDCard &sd,
			   Settings &settings) :
  SDCardAction(name, sd),
  SettingsMenu(settings) {
}


SDListAction::SDListAction(Configurable &menu, const char *name,
			   SDCard &sd, Settings &settings) : 
  SDCardAction(menu, name, sd),
  SettingsMenu(settings) {
}


void SDListAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  SDC.listFiles(SettingsMenu.path(), false, stream);
  stream.println();
}


void SDRemoveAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  if (! SDC.exists(SettingsMenu.path())) {
    stream.printf("Folder \"%s\" does not exist.\n\n", SettingsMenu.path());
    return;
  }
  stream.printf("Erase all files in folder \"%s\".\n", SettingsMenu.path());
  if (Action::yesno("Do you really want to erase all recordings?",
		    true, stream))
    SDC.removeFiles(SettingsMenu.path(), stream);
  stream.println();
}


RTCAction::RTCAction(const char *name, RTClock &rtclock) :
  RTCAction(*root()->Config, name, rtclock) {
}


RTCAction::RTCAction(Configurable &menu, const char *name, RTClock &rtclock) :
  Action(menu, name, StreamInput),
  RTC(rtclock) {
}


void ReportRTCAction::configure(Stream &stream, unsigned long timeout) {
  char times[20];
  RTC.dateTime(times);
  stream.printf("Current time: %s\n\n", times);
}


void SetRTCAction::configure(Stream &stream, unsigned long timeout) {
  RTC.set(stream);
  stream.println();
}

