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


SaveAction::SaveAction(const char *name, SDCard &sd) :
  SaveAction("Save configuration", sd, *Configurator::Config) {
}


SaveAction::SaveAction(const char *name, SDCard &sd, Configurable &config) :
  Action("Save configuration", StreamIO),
  Config(&config),
  SDC(&sd) {
  Config->add(this);
}


void SaveAction::execute() {
  Serial.print("Save configuration to SD card ...");
  //Config->save(*SDC);
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


void Configurator::report(Stream &stream, size_t indent,
			  size_t w, bool descend) const {
  Config->report(stream, indent, w, descend);
}


bool Configurator::save(SDCard &sd) const {
  return Configurable::save(sd, ConfigFile);
}


void Configurator::configure(Stream &stream, unsigned long timeout) {
  Configurable::configure(stream, timeout);
}


void Configurator::configure(SDCard &sd) {
  Configurable::configure(sd, ConfigFile);
}

