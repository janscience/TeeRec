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
  Configurator::MainConfig->Config = this;
}


ReportConfigAction::ReportConfigAction(const char *name) :
  ReportConfigAction(*Configurator::MainConfig->Config, name) {
}


ReportConfigAction::ReportConfigAction(Configurable &menu, const char *name) :
  Action(menu, name, StreamInput) {
}


void ReportConfigAction::configure(Stream &stream, unsigned long timeout) {
  Configurator::MainConfig->report(stream);
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
  if (Configurator::MainConfig->save(SDC))
    stream.printf("Saved configuration to file \"%s\" on SD card.\n",
		  Configurator::MainConfig->configFile());
  stream.println();
}


void LoadConfigAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  bool r = Action::yesno("Do you really want to reload the configuration file?",
			 true, stream);
  stream.println();
  if (r)
    Configurator::MainConfig->load(SDC);
}


void RemoveConfigAction::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(Configurator::MainConfig->configFile())) {
    stream.printf("Configuration file \"%s\" does not exist on SD card.\n\n",
		  Configurator::MainConfig->configFile());
    return;
  }
  if (Action::yesno("Do you really want to remove the configuration file?",
		    true, stream)) {
    if (SDC.remove(Configurator::MainConfig->configFile()))
      stream.printf("\nRemoved configuration file \"%s\" from SD card.\n\n",
		    Configurator::MainConfig->configFile());
    else
      stream.printf("\nERROR! Failed to remove configuration file \"%s\" from SD card.\n\n",
		    Configurator::MainConfig->configFile());
  }
  else
    stream.println();
}


RTCAction::RTCAction(const char *name, RTClock &rtclock) :
  RTCAction(*Configurator::MainConfig->Config, name, rtclock) {
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

