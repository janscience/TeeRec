#include <Configurator.h>
#include <Configurable.h>


Configurable::Configurable(const char *name) :
  NParams(0) {
  setName(name);
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
  Configured = false;
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
  // TODO: check for width of all keys and format accordingly
  Serial.printf("%s settings:\n", name());
  for (size_t j=0; j<NParams; j++) {
    char pval[Parameter::MaxVal];
    Params[j]->valueStr(pval);
    Serial.printf("  %s: %s\n", Params[j]->key(), pval);
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


void Configurable::setName(const char *name) {
  strncpy(ConfigName, name, MaxName-1);
}


float Configurable::parseTime(const char *val) {
  float time = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 's')
      break;
    else if (c == 'h') {
      time *= 60.0*60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'i') {
      time *= 60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 0.001;
      break;
    }
    else if (c == 'u' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-6;
      break;
    }
    else if (c == 'n' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-9;
      break;
    }
  }
  return time;
}


float Configurable::parseFrequency(const char *val) {
  float freq = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 'h' && k<strlen(val)-1 && tolower(val[k+1]) == 'z')
      break;
    else if (c == 'k' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1000.0;
      break;
    }
    else if (val[k] == 'M' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1e6;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 0.001;
      break;
    }
  }
  return freq;
}

