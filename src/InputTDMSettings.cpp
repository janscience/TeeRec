#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(uint32_t rate, int nchannels, float gain) :
  Configurable("ADC"),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  TDM(0) {
}


InputTDMSettings::InputTDMSettings(InputTDM *tdm, uint32_t rate, int nchannels,
				   float gain) :
  Configurable("ADC"),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  TDM(tdm) {
}


InputTDMSettings::InputTDMSettings(const char *name, uint32_t rate,
				   int nchannels, float gain) :
  Configurable(name),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  TDM(0) {
}


InputTDMSettings::InputTDMSettings(InputTDM *tdm, const char *name,
				   uint32_t rate, int nchannels, float gain) :
  Configurable(name),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  TDM(tdm) {
}


void InputTDMSettings::setRate(uint32_t rate) {
  Rate = rate;
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels = nchannels;
}


void InputTDMSettings::setGain(float gain) {
  Gain = gain;
}


void InputTDMSettings::configure(const char *key, const char *val) {
  char pval[30];
  if (strcmp(key, "samplingrate") == 0) {
    setRate(uint32_t(parseFrequency(val)));
    sprintf(pval, "%luHz", Rate);
    if (TDM != 0)
      TDM->setRate(rate());
  }
  else if (strcmp(key, "nchannels") == 0) {
    setNChannels(atoi(val));
    sprintf(pval, "%d", NChannels);
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


void InputTDMSettings::configure(InputTDM *tdm) {
  if (tdm == 0)
    tdm = TDM;
  if (tdm == 0)
    return;
  tdm->setRate(rate());
}


void InputTDMSettings::setConfiguration(InputTDM *tdm) {
  if (tdm == 0)
    tdm = TDM;
  if (tdm == 0)
    return;
  setRate(tdm->rate());
}


void InputTDMSettings::report() const {
  Serial.printf("%s settings:\n", name());
  Serial.printf("  samplingrate : %.1fkHz\n", 0.001*Rate);
  Serial.printf("  nchannels    : %d\n", NChannels);
  Serial.printf("  gain         : %.1f\n", Gain);
}
