#include <Configurable.h>
#include <Parameter.h>


Parameter::Parameter(Configurable *cfg, const char *key) :
  Enabled(true) {
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

  // find out order of old unit:
  int k = 0;
  for (k=0; k<NUnits; k++)
    if (strncmp(oldunit, UnitPref[k], strlen(UnitPref[k])) == 0)
      break;
  float f1 = 1.0;
  if (k < NUnits && strlen(UnitPref[k]) < strlen(oldunit))
    f1 = UnitFac[k];
  else if (strcmp(oldunit, "%") == 0)
    f1 = 0.01;
  else if (strcmp(oldunit, "hour") == 0 || strcmp(oldunit, "h") == 0)
    f1 = 60.0*60.0;
  else if (strcmp(oldunit, "min") == 0)
    f1 = 60.0;

  // find out order of new unit:
  for (k=0; k<NUnits; k++)
    if (strncmp(newunit, UnitPref[k], strlen(UnitPref[k])) == 0)
      break;
  float f2 = 1.0;
  if (k < NUnits && strlen(UnitPref[k]) < strlen(newunit))
    f2 = UnitFac[k];
  else if (strcmp(newunit, "%") == 0)
    f1 = 100.0;
  else if (strcmp(newunit, "hour") == 0 || strcmp(newunit, "h") == 0)
    f1 = 1.0/60.0/60.0;
  else if (strcmp(newunit, "min") == 0)
    f1 = 1.0/60.0;

  return val * f1/f2;
}
