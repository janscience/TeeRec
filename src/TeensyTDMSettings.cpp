#include <TeensyTDM.h>
#include <TeensyTDMSettings.h>


TeensyTDMSettings::TeensyTDMSettings(uint32_t rate, float gain) :
  Configurable("TDM"),
  Rate(rate),
  Gain(gain) {
}


TeensyTDMSettings::TeensyTDMSettings(const char *name, uint32_t rate,
				     float gain) :
  Configurable(name),
  Rate(rate),
  Gain(gain) {
}


void TeensyTDMSettings::setRate(uint32_t rate) {
  Rate = rate;
}


void TeensyTDMSettings::setGain(float gain) {
  Gain = gain;
}


void TeensyTDMSettings::configure(const char *key, const char *val) {
  char pval[30];
  if (strcmp(key, "samplingrate") == 0) {
    setRate(uint32_t(parseFrequency(val)));
    sprintf(pval, "%luHz", Rate);
  }
  else if (strcmp(key, "gain") == 0) {
    setGain(atof(val));
    sprintf(pval, "%.1f", Gain);
  }
  else {
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
    return;
  }
  Serial.printf("  set %s-%s to %s\n", name(), key, pval);
}


void TeensyTDMSettings::report() const {
  Serial.printf("%s settings:\n", name());
  Serial.printf("        %.1fkHz\n", 0.001*Rate);
  Serial.printf("  gain: %.1f\n", Gain);
}
