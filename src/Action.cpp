#include <SDWriter.h>
#include <Configurable.h>
#include <Action.h>


Action::Action(const char *name, int roles) :
  SupportedRoles(roles),
  Roles(roles),
  Indentation(2) {
  setName(name);
}


Action::Action(Configurable *cfg, const char *name, int roles) :
  Action(name, roles) {
  if (cfg != 0)
    cfg->add(this);
}


void Action::setName(const char *name) {
  strncpy(Name, name, MaxName);
  Name[MaxName-1] = '\0';
}


bool Action::enabled(int roles) const {
  roles &= SupportedRoles;
  return (Roles & roles == roles);
}


void Action::enable(int roles) {
  roles &= SupportedRoles;
  Roles |= roles;
}


void Action::disable(int roles) {
  roles &= SupportedRoles;
  Roles &= ~roles;
}


void Action::disableSupported(int roles) {
  roles &= SupportedRoles;
  SupportedRoles &= ~roles;
  Roles = SupportedRoles;
}


void Action::report(Stream &stream, size_t indent,
		    size_t w, bool descend) const {
  if (enabled(StreamOutput))
    Serial.printf("%*s%s ...\n", indent, "", name());
}

