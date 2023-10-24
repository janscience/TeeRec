#include <Configurator.h>
#include <Configurable.h>


Configurable::Configurable(const char *name) :
  NParams(0) {
  setName(name);
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
  Configured = false;
}


void Configurable::setName(const char *name) {
  strncpy(ConfigName, name, MaxName-1);
}


void Configurable::add(Parameter *param) {
  if (NParams >= MaxParams) {
    Serial.printf("ERROR! Number of maximum Parameter exceeded in %s!\n", ConfigName);
    return;
  }
  Params[NParams++] = param;
}


Parameter *Configurable::parameter(const char *key) {
  char lkey[strlen(key)+1];
  for (size_t k=0; k<strlen(key)+1; k++)
    lkey[k] = tolower(key[k]);
  for (size_t j=0; j<NParams; j++) {
    char ckey[strlen(Params[j]->key())+1];
    for (size_t k=0; k<strlen(Params[j]->key())+1; k++)
      ckey[k] = tolower(Params[j]->key()[k]);
    if (strcmp(ckey, lkey) == 0)
      return Params[j];
  }
  return NULL;
}


void Configurable::enable(const char *key) {
  Parameter *param = parameter(key);
  if (param != NULL)
    param->enable();
}


void Configurable::disable(const char *key) {
  Parameter *param = parameter(key);
  if (param != NULL)
    param->disable();
}


void Configurable::report() const {
  // longest key:
  size_t w = 0;
  for (size_t j=0; j<NParams; j++) {
    if (Params[j]->enabled() && strlen(Params[j]->key()) > w)
      w = strlen(Params[j]->key());
  }
  // write parameters to serial:
  Serial.printf("%s settings:\n", name());
  for (size_t j=0; j<NParams; j++) {
    if (Params[j]->enabled()) {
      char pval[Parameter::MaxVal];
      Params[j]->valueStr(pval);
      Serial.printf("  %-*s: %s\n", w, Params[j]->key(), pval);
    }
  }
}


void Configurable::configure(const char *key, const char *val) {
  Parameter *param = parameter(key);
  if (param == NULL) {
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
  }
  else {
    param->parseValue(val);
    char pval[Parameter::MaxVal];
    param->valueStr(pval);
    Serial.printf("  set %s-%s to %s\n", name(), key, pval);
  }
}


void Configurable::save(File &file) const {
  // longest key:
  size_t w = 0;
  for (size_t j=0; j<NParams; j++) {
    if (Params[j]->enabled() && strlen(Params[j]->key()) > w)
      w = strlen(Params[j]->key());
  }
  // write parameters to file:
  file.printf("%s:\n", name());
  for (size_t j=0; j<NParams; j++) {
    if (Params[j]->enabled()) {
      char pval[Parameter::MaxVal];
      Params[j]->valueStr(pval);
      file.printf("  %-*s: %s\n", w, Params[j]->key(), pval);
    }
  }
}


