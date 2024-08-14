#include <RTClock.h>
#include <SDWriter.h>
#include <Configurator.h>
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
		    true, stream)) {
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

