#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(uint32_t rate, int nchannels, float gain) :
  Configurable("ADC"),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  RateP(this, "SamplingRate", &Rate, "%lu"),
  NChannelsP(this, "NChannels", &NChannels, "%hu"),
  GainP(this, "Gain", &Gain, "%.1f", "dB") {
}


InputTDMSettings::InputTDMSettings(const char *name, uint32_t rate,
				   int nchannels, float gain) :
  Configurable(name),
  Rate(rate),
  NChannels(nchannels),
  Gain(gain),
  RateP(this, "SamplingRate", &Rate, "%lu"),
  NChannelsP(this, "NChannels", &NChannels, "%hu"),
  GainP(this, "Gain", &Gain, "%.1f", "dB") {
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


void InputTDMSettings::configure(InputTDM *tdm) {
  if (tdm == 0)
    return;
  tdm->setRate(rate());
}


void InputTDMSettings::setConfiguration(InputTDM *tdm) {
  if (tdm == 0)
    return;
  setRate(tdm->rate());
}
