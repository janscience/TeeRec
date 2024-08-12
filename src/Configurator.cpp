#include <SDWriter.h>
#include <Configurator.h>


ConfigureAction::ConfigureAction(const char *name) :
  ConfigureAction(name, *Configurator::Config) {
}


ConfigureAction::ConfigureAction(const char *name, Configurable &config) :
  Configurable(NULL, name) {
  config.add(this);
  Configurator::Config = this;
  disableSupported(FileIO);
  disableSupported(StreamOutput);
}


ReportAction::ReportAction(const char *name) :
  ReportAction(name, *Configurator::Config) {
}


ReportAction::ReportAction(const char *name, Configurable &config) :
  Action(name, StreamIO),
  Config(&config) {
  Config->add(this);
}


void ReportAction::execute() {
  Config->report(Serial);
  Serial.println();
}


SaveAction::SaveAction(const char *name, SDCard &sd, Configurator &config) :
  SaveAction(name, sd, config, *Configurator::Config) {
}

SaveAction::SaveAction(const char *name, SDCard &sd, Configurator &config,
		       Configurable &menu) :
  Action(name, StreamIO),
  Config(&config),
  SDC(&sd) {
  menu.add(this);
}


void SaveAction::execute() {
  if (Config->save(*SDC))
    Serial.printf("Saved configuration to file \"%s\" on SD card.\n",
		  Config->configFile());
  Serial.println();
}


LoadAction::LoadAction(const char *name, SDCard &sd, Configurator &config) :
  LoadAction(name, sd, config, *Configurator::Config) {
}

LoadAction::LoadAction(const char *name, SDCard &sd, Configurator &config,
		       Configurable &menu) :
  Action(name, StreamIO),
  Config(&config),
  SDC(&sd) {
  menu.add(this);
}


void LoadAction::execute() {
  if (disabled(StreamInput))
    return;
  bool r = Action::yesno("Do you really want to reload the configuration file?",
			 true, Serial);
  Serial.println();
  if (r)
    Config->configure(*SDC);
}


Configurable *Configurator::Config = NULL;


Configurator::Configurator() :
  Configurator("Menu") {
}


Configurator::Configurator(const char *name) :
  Configurable(name) {
  Config = this;
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

