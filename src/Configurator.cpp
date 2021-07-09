#include <SDWriter.h>
#include <Configurable.h>
#include <Configurator.h>


Configurator *Configurator::Config = NULL;


Configurator::Configurator() {
  Config = this;
  NConfigs = 0;
  ConfigFile = new char[11];
  strcpy(ConfigFile, "teerec.cfg");
}


void Configurator::add(Configurable *config) {
  if (NConfigs >= MaxConfigs) {
    Serial.println("ERROR in Configurator: too many Configurables");
    return;
  }
  Configs[NConfigs++] = config;
}


Configurable *Configurator::configurable(const char *name) {
  char lname[strlen(name)+1];
  for (size_t k=0; k<strlen(name)+1; k++)
    lname[k] = tolower(name[k]);
  for (size_t k=0; k<NConfigs; k++) {
    if (strcmp(Configs[k]->name(), lname) == 0)
      return Configs[k];
  }
  return NULL;
}


void Configurator::setConfigFile(const char *fname) {
  if (ConfigFile != NULL)
    delete [] ConfigFile;
  ConfigFile = new char[strlen(fname)+1];
  strcpy(ConfigFile, fname);
}


void Configurator::configure(SDCard &sd) {
  Configurable *config = NULL;
  char line[100];
  FsFile file = sd.open(ConfigFile, FILE_READ);
  if (!file.isOpen() || file.available() < 10) {
    Serial.printf("Configuration file \"%s\" not found or empty.\n", ConfigFile);
    return;
  }
  Serial.printf("Read configuration file \"%s\" ...\n", ConfigFile);
  while (file.available()) {
    file.fgets(line, sizeof(line));
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
	config = configurable(key);
	if (config == NULL)
	  Serial.printf("  no configuration candidate for section \"%s\" found.\n", key);
      }
      else if (config) {
	for (int i=strlen(val)-1; i>=0; i--) {
	  if (val[i] != ' ') {
	    val[i+1] = '\0';
	    break;
	  }
	}
	config->configure(key, val);
      }
    }
  }
  file.close();
  Serial.println();
}

