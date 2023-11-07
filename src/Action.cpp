#include <SDWriter.h>
#include <Configurable.h>
#include <Action.h>


Action::Action(const char *name) :
  Enabled(true) {
  setName(name);
}


Action::Action(Configurable *cfg, const char *name) :
  Action(name) {
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


void Action::report(size_t indent, size_t w, bool descend) const {
  Serial.printf("%*s%s\n", indent, "", name());
}

