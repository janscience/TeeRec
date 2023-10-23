#include <Configurable.h>
#include <Parameter.h>


Parameter::Parameter(Configurable *cfg, const char *key) :
  Enabled(true) {
  setKey(key);
  if (cfg != 0)
    cfg->add(this);
}


void Parameter::setKey(const char *key) {
  strncpy(Key, key, MaxKey);
}


void Parameter::enable() {
  Enabled = true;
}


void Parameter::disable() {
  Enabled = false;
}


StringParameter::StringParameter(Configurable *cfg, const char *key,
				 char **str, size_t n) :
  Parameter(cfg, key),
  MaxStr(n),
  Str(str) {
}


void StringParameter::parseValue(const char *val) {
  if (disabled())
    return;
  strncpy(*Str, val, MaxStr);
}


void StringParameter::valueStr(char *str) {
  strncpy(str, *Str, MaxStr);
}

