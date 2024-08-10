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


Action *Action::action(const char *name) {
  char lname[strlen(name)+1];
  for (size_t k=0; k<strlen(name)+1; k++)
    lname[k] = tolower(name[k]);
  char cname[strlen(Name)+1];
  for (size_t k=0; k<strlen(Name)+1; k++)
    cname[k] = tolower(Name[k]);
  if (strcmp(cname, lname) == 0)
    return this;
  return NULL;
}


bool Action::enabled(int roles) const {
  roles &= SupportedRoles;
  return ((Roles & roles) == roles);
}


bool Action::disabled(int roles) const {
  return !enabled(roles);
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
    stream.printf("%*s%s ...\n", indent, "", name());
}


void Action::configure(Stream &stream, unsigned long timeout) {
  execute();
}
