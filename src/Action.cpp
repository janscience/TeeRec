#include <SDWriter.h>
#include <Configurable.h>
#include <Configurator.h>
#include <Action.h>


bool Action::yesno(const char *request, bool defval,
		   bool echo, Stream &stream) {
  while (true) {
    stream.print(request);
    if (defval)
      stream.print(" [Y/n] ");
    else
      stream.print(" [y/N] ");
    while (stream.available() == 0)
      yield();
    char pval[8];
    stream.readBytesUntil('\n', pval, 8);
    if (echo)
      stream.println(pval);
    if (strlen(pval) == 0)
      return defval;
    if (tolower(pval[0]) == 'y')
      return true;
    if (tolower(pval[0]) == 'n')
      return false;
  }
}


Action::Action(const char *name, int roles) :
  SupportedRoles(roles),
  Roles(roles),
  Indentation(2),
  Parent(NULL) {
  setName(name);
}


Action::Action(Configurable &menu, const char *name, int roles) :
  Action(name, roles) {
  menu.add(this);
}


void Action::setName(const char *name) {
  strncpy(Name, name, MaxName);
  Name[MaxName-1] = '\0';
}


Configurator *Action::root() {
  Action *act = this;
  while (act->parent() != NULL)
    act = act->parent();
  return static_cast<Configurator*>(act);
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
  if (roles == 0)
    return false;
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
  if (enabled(StreamOutput) || enabled(StreamInput))
    stream.printf("%*s%s\n", indent, "", name());
}


void Action::configure(Stream &stream, unsigned long timeout,
		       bool echo, bool detailed) {
}
