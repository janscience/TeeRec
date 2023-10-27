#include <SDWriter.h>
#include <Configurable.h>
#include <Parameter.h>


Parameter::Parameter(Configurable *cfg, const char *key) :
  Enabled(true),
  NSelection(0) {
  setKey(key);
  if (cfg != 0)
    cfg->add(this);
}


void Parameter::setKey(const char *key) {
  strncpy(Key, key, MaxKey);
  Key[MaxKey-1] = '\0';
}


void Parameter::enable() {
  Enabled = true;
}


void Parameter::disable() {
  Enabled = false;
}


void Parameter::configure(Stream &stream, unsigned long timeout) {
  if (disabled())
    return;
  int w = strlen(key());
  if (w < 15)
    w = 15;
  char pval[MaxVal];
  valueStr(pval);
  stream.printf("%-*s: %s\n", w, key(), pval);
  while (true) {
    stream.printf("%-*s: ", w, "enter new value");
    elapsedMillis time = 0;
    while ((stream.available() == 0) && (timeout == 0 || time < timeout)) {
      yield();
    }
    stream.readBytesUntil('\n', pval, MaxVal);
    stream.println(pval);
    if (strcmp(pval, "l") == 0 && NSelection > 0)
      listSelection(stream);
    else if (parseValue(pval))
      break;
  }
  stream.println();
}


void Parameter::configure(const char *val, const char *name) {
  if (disabled())
    return;
  char keyname[2*MaxKey];
  keyname[0] = '\0';
  if (name != 0 && strlen(name) > 0) {
    strcat(keyname, name);
    strcat(keyname, "-");
  }
  strcat(keyname, key());
  if (parseValue(val)) {
    char pval[MaxVal];
    valueStr(pval);
    Serial.printf("  set %s to %s\n", keyname, pval);
  }
  else
    Serial.printf("  %s is not a valid value for %s\n", val, keyname);
}


void Parameter::report(int w) const {
  if (enabled()) {
    char pval[MaxVal];
    valueStr(pval);
    Serial.printf("  %-*s: %s\n", w, key(), pval);
  }
}


void Parameter::save(File &file, int w) const {
  if (enabled()) {
    char pval[MaxVal];
    valueStr(pval);
    file.printf("  %-*s: %s\n", w, key(), pval);
  }
}


char Parameter::UnitPref[NUnits][6] = {
  "Deka", "deka", "Hekto", "hekto", "kilo", "Kilo", 
  "Mega", "mega", "Giga", "giga", "Tera", "tera", 
  "Peta", "peta", "Exa", "exa", 
  "Dezi", "dezi", "Zenti", "centi", "Micro", "micro", 
  "Milli", "milli", "Nano", "nano", "Piko", "piko", 
  "Femto", "femto", "Atto", "atto", 
  "da", "h", "K", "k", "M", "G", "T", "P", "E", 
  "d", "c", "mu", "u", "m", "n", "p", "f", "a"
};


float Parameter::UnitFac[NUnits] = {
  1e1,  1e1,  1e2,  1e2,  1e3,  1e3,  
  1e6,  1e6,  1e9,  1e9,  1e12, 1e12, 
  1e15, 1e15, 1e18, 1e18,
  1e-1, 1e-1, 1e-2, 1e-2, 1e-6, 1e-6, 
  1e-3, 1e-3, 1e-9, 1e-9, 1e-12, 1e-12, 
  1e-15, 1e-15, 1e-18, 1e-18, 
  1e1,  1e2,  1e3,  1e3,  1e6,  1e9, 1e12, 1e15, 1e18,
  1e-1, 1e-2, 1e-6, 1e-6, 1e-3, 1e-9, 1e-12, 1e-15, 1e-18
};


float Parameter::changeUnit(float val, const char *oldunit,
			    const char *newunit) {
  // adapted from https://github.com/relacs/relacs/blob/1facade622a80e9f51dbf8e6f8171ac74c27f100/options/src/parameter.cc#L1647-L1703

  // missing unit?
  if (newunit == 0 || strlen(newunit) == 0 ||
      oldunit == 0 || strlen(oldunit) == 0)
    return val;

  // parse old unit:
  float f1 = 1.0;
  if (strcmp(oldunit, "%") == 0)
    f1 = 0.01;
  else if (strcmp(oldunit, "hour") == 0 || strcmp(oldunit, "h") == 0)
    f1 = 60.0*60.0;
  else if (strcmp(oldunit, "min") == 0)
    f1 = 60.0;
  else {
    int k = 0;
    for (k=0; k<NUnits; k++)
      if (strncmp(oldunit, UnitPref[k], strlen(UnitPref[k])) == 0)
	break;
    if (k < NUnits && strlen(UnitPref[k]) < strlen(oldunit))
      f1 = UnitFac[k];
  }
  
  // parse new unit:
  float f2 = 1.0;
  if (strcmp(newunit, "%") == 0)
    f2 = 100.0;
  else if (strcmp(newunit, "hour") == 0 || strcmp(newunit, "h") == 0)
    f2 = 1.0/60.0/60.0;
  else if (strcmp(newunit, "min") == 0)
    f2 = 1.0/60.0;
  else {
    int k = 0;
    for (k=0; k<NUnits; k++)
      if (strncmp(newunit, UnitPref[k], strlen(UnitPref[k])) == 0)
	break;
    if (k < NUnits && strlen(UnitPref[k]) < strlen(newunit))
      f2 = UnitFac[k];
  }
  
  return val * f1/f2;
}


BaseStringParameter::BaseStringParameter(Configurable *cfg, const char *key) :
  Parameter(cfg, key),
  Selection(0) {
}


void BaseStringParameter::setSelection(const char **selection, size_t n) {
  NSelection = n;
  Selection = selection;
}


int BaseStringParameter::checkSelection(const char *val) {
  if (NSelection == 0)
    return 0;
  for (size_t k=0; k<NSelection; k++)
    if (strcmp(Selection[k], val) == 0)
      return k;
  return -1;
}


void BaseStringParameter::listSelection(Stream &stream) const {
  for (size_t k=0; k<NSelection; k++)
    stream.printf("  - %s\n", Selection[k]);
}

