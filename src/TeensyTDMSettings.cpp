#include <TeensyTDM.h>
#include <TeensyTDMSettings.h>


TeensyTDMSettings::TeensyTDMSettings(uint32_t rate, float gain) :
  Configurable("ADC"),
  Rate(rate),
  Gain(gain),
  TDM(0) {
}


TeensyTDMSettings::TeensyTDMSettings(TeensyTDM *tdm, uint32_t rate,
				     float gain) :
  Configurable("ADC"),
  Rate(rate),
  Gain(gain),
  TDM(tdm) {
}


TeensyTDMSettings::TeensyTDMSettings(const char *name, uint32_t rate,
				     float gain) :
  Configurable(name),
  Rate(rate),
  Gain(gain),
  TDM(0) {
}


TeensyTDMSettings::TeensyTDMSettings(TeensyTDM *tdm, const char *name,
				     uint32_t rate, float gain) :
  Configurable(name),
  Rate(rate),
  Gain(gain),
  TDM(tdm) {
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
    if (TDM != 0)
      TDM->setRate(rate());
  }
  else if (strcmp(key, "gain") == 0) {
    setGain(atof(val));
    sprintf(pval, "%.1f", Gain);
    /*
    if (TDM != 0)
      TDM->setGain(gain());
    */
  }
  else {
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
    return;
  }
  Serial.printf("  set %s-%s to %s\n", name(), key, pval);
}


void TeensyTDMSettings::configure(TeensyTDM *tdm) {
  if (tdm == 0)
    tdm = TDM;
  if (tdm == 0)
    return;
  tdm->setRate(rate());
  //tdm->setGain(gain());
}


void TeensyTDMSettings::setConfiguration(TeensyTDM *tdm) {
  if (tdm == 0)
    tdm = TDM;
  if (tdm == 0)
    return;
  setRate(tdm->rate());
  //  setGain(tdm->gain());
}


void TeensyTDMSettings::report() const {
  Serial.printf("%s settings:\n", name());
  Serial.printf("        %.1fkHz\n", 0.001*Rate);
  Serial.printf("  gain: %.1f\n", Gain);
}
