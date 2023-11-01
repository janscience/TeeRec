#include <SDWriter.h>
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
    Serial.printf("ERROR! Number of maximum Parameter exceeded in %s!\n",
		  name());
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
  Serial.printf("%s:\n", name());
  for (size_t j=0; j<NParams; j++)
    Params[j]->report(w);
}


void Configurable::configure(Stream &stream, unsigned long timeout) {
  int def = 0;
  while (true) {
    stream.printf("%s:\n", name());
    char pval[Parameter::MaxVal];
    size_t iparam[NParams];
    size_t n = 0;
    for (size_t j=0; j<NParams; j++) {
      if (Params[j]->enabled()) {
	Params[j]->valueStr(pval);
	stream.printf("  %d) %s: %s\n", n+1, Params[j]->key(), pval);
	iparam[n++] = j;
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
      stream.readBytesUntil('\n', pval, Parameter::MaxVal);
      if (strlen(pval) == 0)
	sprintf(pval, "%d", def+1);
      stream.println(pval);
      char *end;
      long i = strtol(pval, &end, 10) - 1;
      if (end != pval && i >= 0 && i < (long)n &&
	  iparam[i] < NParams) {
	def = i;
	stream.println();
	Params[iparam[i]]->configure(stream, timeout);
	break;
      }
      else if (strcmp(pval, "q") == 0) {
	stream.println();
	return;
      }
    }
  }
}


void Configurable::configure(const char *key, const char *val) {
  Parameter *param = parameter(key);
  if (param == NULL)
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
  else
    param->configure(val, name());
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
  for (size_t j=0; j<NParams; j++)
    Params[j]->save(file, w);
}


