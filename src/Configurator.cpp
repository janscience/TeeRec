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
  Config->report();
  Serial.println();
}


SaveAction::SaveAction(const char *name, SDCard &sd, Configurator &config) :
  SaveAction("Save configuration", sd, config, *Configurator::Config) {
}

SaveAction::SaveAction(const char *name, SDCard &sd, Configurator &config,
		       Configurable &menu) :
  Action("Save configuration", StreamIO),
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
}


void Configurator::setConfigFile(const char *fname) {
  strncpy(ConfigFile, fname, MaxFile);
  ConfigFile[MaxFile-1] = '\0';
}


void Configurator::report(Stream &stream) const {
  Config->report(stream, 0, 0, true);
}


bool Configurator::save(SDCard &sd) const {
  return Config->save(sd, ConfigFile);
}


void Configurator::configure(Stream &stream, unsigned long timeout) {
  Configurable::configure(stream, timeout);
}


void Configurator::configure(SDCard &sd) {
  Configurable::configure(sd, ConfigFile);
}

