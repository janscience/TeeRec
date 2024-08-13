#include <SDWriter.h>
#include <Configurator.h>


ConfigureAction::ConfigureAction(const char *name) :
  ConfigureAction(name, *Configurator::Config) {
}


ConfigureAction::ConfigureAction(const char *name, Configurable &menu) :
  Configurable(NULL, name) {
  menu.add(this);
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
  Action(name, StreamIO),
  SDC(sd) {
}


SaveConfigAction::SaveConfigAction(const char *name, SDCard &sd) :
  SaveConfigAction(name, sd, *Configurator::Config) {
}


SaveConfigAction::SaveConfigAction(const char *name, SDCard &sd,
				   Configurable &menu) :
  SDCardAction(name, sd) {
  menu.add(this);
}


void SaveConfigAction::execute() {
  if (Configurator::MainConfig->save(SDC))
    Serial.printf("Saved configuration to file \"%s\" on SD card.\n",
		  Configurator::MainConfig->configFile());
  Serial.println();
}


LoadConfigAction::LoadConfigAction(const char *name, SDCard &sd) :
  LoadConfigAction(name, sd, *Configurator::Config) {
}


LoadConfigAction::LoadConfigAction(const char *name, SDCard &sd,
				   Configurable &menu) :
  SDCardAction(name, sd) {
  menu.add(this);
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


RemoveConfigAction::RemoveConfigAction(const char *name, SDCard &sd) :
  RemoveConfigAction(name, sd, *Configurator::Config) {
}


RemoveConfigAction::RemoveConfigAction(const char *name, SDCard &sd,
				       Configurable &menu) :
  SDCardAction(name, sd) {
  menu.add(this);
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


Configurable *Configurator::Config = NULL;
Configurator *Configurator::MainConfig = NULL;


Configurator::Configurator() :
  Configurator("Menu") {
}


Configurator::Configurator(const char *name) :
  Configurable(name) {
  Config = this;
  MainConfig = this;
  strncpy(ConfigFile, "teerec.cfg", MaxFile);
  ConfigFile[MaxFile-1] = '\0';
  disableSupported(StreamOutput);
  disableSupported(FileIO);
}


void Configurator::setConfigFile(const char *fname) {
  strncpy(ConfigFile, fname, MaxFile);
  ConfigFile[MaxFile-1] = '\0';
}


void Configurator::report(Stream &stream, size_t indent,
			  size_t w, bool descend) const {
  Config->Configurable::report(stream, indent, w, descend);
}


bool Configurator::save(SDCard &sd) const {
  return Config->Configurable::save(sd, ConfigFile);
}


void Configurator::configure(Stream &stream, unsigned long timeout) {
  Configurable::configure(stream, timeout);
}


void Configurator::configure(SDCard &sd) {
  Config->Configurable::configure(sd, ConfigFile);
}

