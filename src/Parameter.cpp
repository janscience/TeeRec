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
  Key[MaxKey-1] = '\0';
}


void Parameter::enable() {
  Enabled = true;
}


void Parameter::disable() {
  Enabled = false;
}

