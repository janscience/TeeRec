#include <SDWriter.h>
#include <Configurable.h>
#include <Action.h>


Action::Action(Configurable *cfg, const char *name) :
  Enabled(true) {
  setName(name);
  if (cfg != 0)
    cfg->add(this);
}


void Action::setName(const char *name) {
  strncpy(Name, name, MaxName);
  Name[MaxName-1] = '\0';
}


void Action::enable() {
  Enabled = true;
}


void Action::disable() {
  Enabled = false;
}

