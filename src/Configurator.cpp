#include <Configurator.h>


Configurator *Configurator::MainConfig = NULL;


Configurator::Configurator() :
  Configurator("Menu") {
}


Configurator::Configurator(const char *name) :
  Configurable(name),
  Config(this) {
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


void Configurator::load(SDCard &sd) {
  Config->Configurable::load(sd, ConfigFile);
}


void Configurator::configure(Stream &stream, unsigned long timeout,
			     bool detailed) {
  Configurable::configure(stream, timeout, detailed);
}

