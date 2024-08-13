#include <RTClock.h>
#include <SDWriter.h>
#include <Configurator.h>
#include <ToolActions.h>


ConfigureAction::ConfigureAction(const char *name) :
  ConfigureAction(name, *Configurator::Config) {
}


ConfigureAction::ConfigureAction(const char *name, Configurable &menu) :
  Configurable(name, menu) {
  Configurator::Config = this;
  disableSupported(FileIO);
  disableSupported(StreamOutput);
}


ReportConfigAction::ReportConfigAction(const char *name) :
  ReportConfigAction(name, *Configurator::Config) {
}


ReportConfigAction::ReportConfigAction(const char *name, Configurable &menu) :
  Action(name, StreamIO) {
  menu.add(this);
}


void ReportConfigAction::execute() {
  Configurator::MainConfig->report(Serial);
  Serial.println();
}


SDCardAction::SDCardAction(const char *name, SDCard &sd) :
  SDCardAction(name, sd, *Configurator::Config) {
}


SDCardAction::SDCardAction(const char *name, SDCard &sd, Configurable &menu) : 
  Action(name, StreamIO),
  SDC(sd) {
  menu.add(this);
}


void SaveConfigAction::execute() {
  if (Configurator::MainConfig->save(SDC))
    Serial.printf("Saved configuration to file \"%s\" on SD card.\n",
		  Configurator::MainConfig->configFile());
  Serial.println();
}


void LoadConfigAction::execute() {
  if (disabled(StreamInput))
    return;
  bool r = Action::yesno("Do you really want to reload the configuration file?",
			 true, Serial);
  Serial.println();
  if (r)
    Configurator::MainConfig->configure(SDC);
}


void RemoveConfigAction::execute() {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(Configurator::MainConfig->configFile())) {
    Serial.printf("Configuration file \"%s\" does not exist on SD card.\n\n",
		  Configurator::MainConfig->configFile());
    return;
  }
  if (Action::yesno("Do you really want to remove the configuration file?",
		    true, Serial)) {
    if (SDC.remove(Configurator::MainConfig->configFile()))
      Serial.printf("\nRemoved configuration file \"%s\" from SD card.\n\n",
		    Configurator::MainConfig->configFile());
    else
      Serial.printf("\nERROR! Failed to remove configuration file \"%s\" from SD card.\n\n",
		    Configurator::MainConfig->configFile());
  }
  else
    Serial.println();
}


RTCAction::RTCAction(const char *name, RTClock &rtclock) :
  RTCAction(name, rtclock, *Configurator::Config) {
}


RTCAction::RTCAction(const char *name, RTClock &rtclock,
		     Configurable &menu) :
  Action(name, StreamIO),
  RTC(rtclock) {
  menu.add(this);
}


void ReportRTCAction::execute() {
  char times[20];
  RTC.dateTime(times);
  Serial.printf("Current time: %s\n\n", times);
}


void SetRTCAction::execute() {
  RTC.set(Serial);
  Serial.println();
}

