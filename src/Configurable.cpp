#include <Configurator.h>
#include <Configurable.h>


Configurable::Configurable(const char *name) {
  size_t k=0;
  for (; k<strlen(name) && k+1<MaxName; k++)
    ConfigName[k] = tolower(name[k]);
  ConfigName[k] = '\0';
  if (Configurator::Config != NULL)
    Configurator::Config->add(this);
}


void Configurable::configure(const char *key, const char *val) {
  Serial.printf("Requested configuration for \"%s\": %s: %s\n",
		ConfigName, key, val);
}


float Configurable::parseTime(const char *val) {
  float time = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 's')
      break;
    else if (c == 'h') {
      time *= 60.0*60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'i') {
      time *= 60.0;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 0.001;
      break;
    }
    else if (c == 'u' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-6;
      break;
    }
    else if (c == 'n' && k<strlen(val)-1 && tolower(val[k+1]) == 's') {
      time *= 1e-9;
      break;
    }
  }
  return time;
}


float Configurable::parseFrequency(const char *val) {
  float freq = atof(val);
  for (size_t k=0; k<strlen(val); k++) {
    char c = tolower(val[k]);
    if (c == 'h' && k<strlen(val)-1 && tolower(val[k+1]) == 'z')
      break;
    else if (c == 'k' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1000.0;
      break;
    }
    else if (val[k] == 'M' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 1e6;
      break;
    }
    else if (c == 'm' && k<strlen(val)-1 && tolower(val[k+1]) == 'h') {
      freq *= 0.001;
      break;
    }
  }
  return freq;
}

