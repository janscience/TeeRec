#include <SDWriter.h>
#include <Configurator.h>


Configurator *Configurator::Config = NULL;


Configurator::Configurator() :
  Configurator("") {
}


Configurator::Configurator(const char *name) :
  Configurable(name) {
  Config = this;
  strncpy(ConfigFile, "teerec.cfg", MaxFile);
  ConfigFile[MaxFile-1] = '\0';
}


void Configurator::setConfigFile(const char *fname) {
  strncpy(ConfigFile, fname, MaxFile);
  ConfigFile[MaxFile-1] = '\0';
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

