#include <Configurator.h>
#include <Configurable.h>


Configurable::Configurable(const char *name) {
  ConfigName = new char[strlen(name)+1];
  for (size_t k=0; k<strlen(name)+1; k++)
    ConfigName[k] = tolower(name[k]);
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
}


void Configurable::configure(const char *key, const char *val) {
  Serial.printf("Requested configuration for \"%s\": %s: %s\n",
		ConfigName, key, val);
}



