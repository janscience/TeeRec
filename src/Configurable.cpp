#include <SDWriter.h>
#include <Configurator.h>
#include <Configurable.h>


Configurable::Configurable(const char *name) :
  Action(name),
  NActions(0) {
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
  Configured = false;
}


void Configurable::add(Action *act) {
  if (NActions >= MaxActions) {
    Serial.printf("ERROR! Number of maximum Actions exceeded in %s!\n",
		  name());
    return;
  }
  Actions[NActions++] = act;
}


Action *Configurable::action(const char *name) {
  char lname[strlen(name)+1];
  for (size_t k=0; k<strlen(name)+1; k++)
    lname[k] = tolower(name[k]);
  for (size_t j=0; j<NActions; j++) {
    char cname[strlen(Actions[j]->name())+1];
    for (size_t k=0; k<strlen(Actions[j]->name())+1; k++)
      cname[k] = tolower(Actions[j]->name()[k]);
    if (strcmp(cname, lname) == 0)
      return Actions[j];
  }
  return NULL;
}


void Configurable::enable(const char *name) {
  Action *act = action(name);
  if (act != NULL)
    act->enable();
}


void Configurable::disable(const char *name) {
  Action *act = action(name);
  if (act != NULL)
    act->disable();
}


void Configurable::report(size_t indent, size_t w, bool descend) const {
  // write actions to serial:
  if (descend) {
    // longest name:
    size_t ww = 0;
    for (size_t j=0; j<NActions; j++) {
      if (Actions[j]->enabled() && strlen(Actions[j]->name()) > ww)
	ww = strlen(Actions[j]->name());
    }
    Serial.printf("%*s%s:\n", indent, "", name());
    for (size_t j=0; j<NActions; j++)
      Actions[j]->report(indent + 2, ww, descend);
  }
  else
    Serial.printf("%*s%s\n", indent, "", name());
}


void Configurable::save(File &file, size_t indent, size_t w) const {
  // longest name:
  size_t ww = 0;
  for (size_t j=0; j<NActions; j++) {
    if (Actions[j]->enabled() && strlen(Actions[j]->name()) > ww)
      ww = strlen(Actions[j]->name());
  }
  // write actions to file:
  file.printf("%*s%s:\n", indent, "", name());
  for (size_t j=0; j<NActions; j++)
    Actions[j]->save(file, indent + 2, ww);
}


void Configurable::configure(Stream &stream, unsigned long timeout) {
  int def = 0;
  while (true) {
    stream.printf("%s:\n", name());
    size_t iaction[NActions];
    size_t n = 0;
    for (size_t j=0; j<NActions; j++) {
      if (Actions[j]->enabled()) {
	stream.printf("  %d) ", n+1);
	Actions[j]->report(0, 0, false);
	iaction[n++] = j;
      }
    }
    while (true) {
      stream.printf("  Select [%d]: ", def + 1);
      elapsedMillis time = 0;
      while ((stream.available() == 0) && (timeout == 0 || time < timeout)) {
	yield();
      }
      if (stream.available() == 0) {
	// timeout:
	stream.println('\n');
	return;
      }
      char pval[16];
      stream.readBytesUntil('\n', pval, 16);
      if (strlen(pval) == 0)
	sprintf(pval, "%d", def+1);
      stream.println(pval);
      char *end;
      long i = strtol(pval, &end, 10) - 1;
      if (end != pval && i >= 0 && i < (long)n &&
	  iaction[i] < NActions) {
	def = i;
	stream.println();
	Actions[iaction[i]]->configure(stream, timeout);
	break;
      }
      else if (strcmp(pval, "q") == 0) {
	stream.println();
	return;
      }
    }
  }
}


void Configurable::configure(const char *name, const char *val) {
  Action *act = action(name);
  if (act == NULL)
    Serial.printf("  %s name \"%s\" not found.\n", this->name(), name);
  else
    act->configure(val, this->name());
}

