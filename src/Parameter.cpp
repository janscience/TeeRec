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


/*
// from https://github.com/relacs/relacs/blob/1facade622a80e9f51dbf8e6f8171ac74c27f100/options/src/parameter.cc#L1647-L1703

const int NUnits = 50;
string UnitPref[NUnits] = { "Deka", "deka", "Hekto", "hekto", "kilo", "Kilo", 
			    "Mega", "mega", "Giga", "giga", "Tera", "tera", 
			    "Peta", "peta", "Exa", "exa", 
			    "Dezi", "dezi", "Zenti", "centi", "Micro", "micro", 
			    "Milli", "milli", "Nano", "nano", "Piko", "piko", 
			    "Femto", "femto", "Atto", "atto", 
			    "da", "h", "K", "k", "M", "G", "T", "P", "E", 
			    "d", "c", "mu", "u", "m", "n", "p", "f", "a" };
float UnitFac[NUnits] = {  1.0,  1.0,  2.0,  2.0,  3.0,  3.0,  
			    6.0,  6.0,  9.0,  9.0,  12.0, 12.0, 
			   15.0, 15.0, 18.0, 18.0,
			   -1.0, -1.0, -2.0, -2.0, -6.0, -6.0, 
			   -3.0, -3.0, -9.0, -9.0, -12.0, -12.0, 
			  -15.0, -15.0, -18.0, -18.0, 
			    1.0,  2.0,  3.0,  3.0,  6.0,  9.0, 12.0, 15.0, 18.0,
			   -1.0, -2.0, -6.0, -6.0, -3.0, -9.0, -12.0, -15.0, -18.0 };

float changeUnit(float val, const Str &oldunit, const Str &newunit) {
  // disect oldUnit into value ov and unit ou:
  double ov = oldunit.number( 1.0 );
  string ou = oldunit.unit();

  // disect newUnit into value nv and unit nu:
  double nv = newunit.number( 1.0 );
  string nu = newunit.unit();

  // missing unit?
  if ( nu.empty() || ou.empty() ) {
    if ( newunit == "1" && oldunit == "%" )
      nv = 100.0;
    else if ( newunit == "%" && oldunit == "1" ) 
      nv = 0.01;
    return val*ov/nv;
  }

  // find out order of old unit:
  int k = 0;
  for (k=0; k<NUnits; k++)
    if (ou.find( UnitPref[k] ) == 0)
      break;
  double e1 = 0.0;
  if (k < NUnits && UnitPref[k].length() < ou.length())
    e1 = UnitFac[k];

  // find out order of new unit:
  for (k=0; k<NUnits; k++)
    if (nu.find( UnitPref[k] ) == 0)
      break;
  double e2 = 0.0;
  if (k < NUnits && UnitPref[k].length() < nu.length())
    e2 = UnitFac[k];

  return val * (ov/nv) * pow(10.0, e1-e2);
}

*/
