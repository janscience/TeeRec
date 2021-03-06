#include <SDWriter.h>
#include <Configurable.h>
#include <Configurator.h>


Configurator *Configurator::Config = NULL;


Configurator::Configurator() {
  Config = this;
  NConfigs = 0;
  strncpy(ConfigFile, "teerec.cfg", MaxFile);
  Configured = false;
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
  strncpy(ConfigFile, fname, MaxFile);
}


void Configurator::configure(SDCard &sd) {
  Configurable *config = NULL;
  const size_t nline = 100;
  char line[nline];
  FsFile file = sd.openRead(ConfigFile);
  if (!file || file.available() < 10) {
    Serial.printf("Configuration file \"%s\" not found or empty.\n\n", ConfigFile);
    Configured = false;
    return;
  }
  Serial.printf("Read configuration file \"%s\" ...\n", ConfigFile);
  while (file.available()) {
#ifdef SDCARD_USE_SDFAT
    file.fgets(line, nline);
#else
    #warning Reading text files not supported by Teensy SD library. Use SdFat instead.
    file.close();
    return;
#endif
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
	config->setConfigured();
      }
    }
  }
  Configured = true;
  file.close();
  Serial.println();
}

