#include <SDWriter.h>
#include <Configurator.h>
#include <Configurable.h>


char Configurable::MenuTitle[MaxName] = "Menu";


Configurable::Configurable(const char *name) :
  Action(name),
  NActions(0) {
  disableSupported(SetValue);
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
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
  size_t inx = strlen(name);
  char lname[strlen(name)+1];
  for (size_t k=0; k<strlen(name)+1; k++) {
    if (name[k] == '>') {
      lname[k] = '\0';
      inx = k + 1;
      break;
    }
    lname[k] = tolower(name[k]);
  }
  for (size_t j=0; j<NActions; j++) {
    char cname[strlen(Actions[j]->name())+1];
    for (size_t k=0; k<strlen(Actions[j]->name())+1; k++)
      cname[k] = tolower(Actions[j]->name()[k]);
    if (strcmp(cname, lname) == 0) {
      if (inx < strlen(name))
	return Actions[j]->action(name + inx);
      else
	return Actions[j];
    }
  }
  return NULL;
}


void Configurable::enable(const char *name, int roles) {
  Action *act = action(name);
  if (act != NULL)
    act->enable(roles);
}


void Configurable::disable(const char *name, int roles) {
  Action *act = action(name);
  if (act != NULL)
    act->disable(roles);
}


void Configurable::setMenuTitle(const char *title) {
  strncpy(MenuTitle, title, MaxName);
  MenuTitle[MaxName-1] = '\0';
}


void Configurable::report(Stream &stream, size_t indent,
			  size_t w, bool descend) const {
  if (disabled(StreamOutput))
    return;
  // write actions to serial:
  if (descend) {
    // longest name:
    size_t ww = 0;
    for (size_t j=0; j<NActions; j++) {
      if (Actions[j]->enabled(StreamOutput) && strlen(Actions[j]->name()) > ww)
	ww = strlen(Actions[j]->name());
    }
    if (strlen(name()) > 0) {
      stream.printf("%*s%s:\n", indent, "", name());
      indent += indentation();
    }
    for (size_t j=0; j<NActions; j++)
      Actions[j]->report(stream, indent, ww, descend);
  }
  else if (strlen(name()) > 0)
    stream.printf("%*s%s\n", indent, "", name());
}


void Configurable::save(File &file, size_t indent, size_t w) const {
  if (disabled(FileOutput))
    return;
  // longest name:
  size_t ww = 0;
  for (size_t j=0; j<NActions; j++) {
    if (Actions[j]->enabled(FileOutput) && strlen(Actions[j]->name()) > ww)
      ww = strlen(Actions[j]->name());
  }
  // write actions to file:
  if (strlen(name()) > 0) {
    file.printf("%*s%s:\n", indent, "", name());
    indent += indentation();
  }
  for (size_t j=0; j<NActions; j++)
    Actions[j]->save(file, indent, ww);
}


bool Configurable::save(SDCard &sd, const char *filename) const {
  File file = sd.openWrite(filename);
  if (!file) {
    Serial.printf("Configuration file \"%s\" cannot be written.\n\n",
		  filename);
    return false;
  }
  save(file);
  file.close();
  return true;
}


void Configurable::configure(Stream &stream, unsigned long timeout) {
  if (disabled(StreamIO))
    return;
  int def = 0;
  while (true) {
    if (strlen(name()) > 0)
      stream.printf("%s:\n", name());
    else
      stream.printf("%s:\n", MenuTitle);
    size_t iaction[NActions];
    size_t n = 0;
    for (size_t j=0; j<NActions; j++) {
      if (Actions[j]->enabled(StreamIO)) {
	stream.printf("  %d) ", n+1);
	Actions[j]->report(stream, 0, 0, false);
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


void Configurable::configure(const char *name, const char *val,
			     Stream &stream) {
  Action *act = action(name);
  if (act == NULL) {
    if (enabled(StreamOutput))
	stream.printf("%*s%s name \"%s\" not found.\n",
		      indentation(), "", this->name(), name);
  }
  else
    act->configure(val, this->name());
}


void Configurable::configure(SDCard &sd, const char *filename) {
  Action *act = NULL;
  const size_t nline = 128;
  char line[nline];
  char sections[nline];
  File file = sd.openRead(filename);
  if (!file || file.available() < 10) {
    Serial.printf("Configuration file \"%s\" not found or empty.\n\n",
		  filename);
    return;
  }
  Serial.printf("Read configuration file \"%s\" ...\n", filename);
  sections[0] = '\0';
  int indent = 0;
  int previndent = -1;
  int nind = -1;
  while (file.available()) {
    file.readBytesUntil('\n', line, nline);
    char *key = NULL;
    char *val = NULL;
    int state = 0;
    for (size_t k=0; line[k] != '\0'; k++) {
      if (line[k] == '#') {
	line[k] = '\0';
	break;
      }
      if (line[k] == '\n' || line[k] == '\r')
	line[k] = ' ';
      switch (state) {
      case 0: if (line[k] != ' ') {
	  indent = k;
	  if (previndent < 0)
	    previndent = indent;
	  line[k] = tolower(line[k]);
	  key = &line[k];
	  state++;
	}
	break;
      case 1: line[k] = tolower(line[k]);
	if (line[k] == ':') {
	  line[k] = '\0';
	  state++;
	  for (int i=k-1; i>=0; i--) {
	    if (line[i] != ' ') {
	      line[i+1] = '\0';
	      break;
	    }
	  }
	}
	break;
      case 2: if (line[k] != ' ') {
	  val = &line[k];
	  state++;
	}
	break;
      }
    }
    if (state > 1) {
      if (val == NULL) {
	if (indent > previndent) {
	  if (nind < 0)
	    nind = indent - previndent;
	  if (strlen(sections) > 0)
	    strcat(sections, ">");
	  strcat(sections, key);
	}
	else {
	  int n = previndent - indent;
	  n /= nind >= 0 ? nind : 2;
	  n += 1;
	  // n sections up:
	  for(int i=strlen(sections)-1; i>=0; i--) {
	    if (sections[i] == '>') {
	      sections[i] = '\0';
	      if (--n == 0)
		break;
	    }
	  }
	  if (n > 0)
	    sections[0] = '\0';
	  // add new section:
	  if (strlen(sections) > 0)
	    strcat(sections, ">");
	  strcat(sections, key);
	}
	previndent = indent;
	act = action(sections);
	if (act == NULL)
	  Serial.printf("  no configuration candidate for section \"%s\" found.\n", sections);
      }
      else if (act) {
	for (int i=strlen(val)-1; i>=0; i--) {
	  if (val[i] != ' ') {
	    val[i+1] = '\0';
	    break;
	  }
	}
	act->configure(key, val);
      }
    }
  }
  file.close();
  Serial.println();
}
