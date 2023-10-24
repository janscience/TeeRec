#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(uint32_t rate, int nchannels, float gain) :
  Configurable("ADC"),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  NChannels(this, "NChannels", nchannels, "%hu"),
  Gain(this, "Gain", gain, "%.1f", "dB") {
}


InputTDMSettings::InputTDMSettings(const char *name, uint32_t rate,
				   int nchannels, float gain) :
  Configurable(name),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  NChannels(this, "NChannels", nchannels, "%hu"),
  Gain(this, "Gain", gain, "%.1f", "dB") {
}


void InputTDMSettings::setRate(uint32_t rate) {
  Rate.setValue(rate);
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setGain(float gain) {
  Gain.setValue(gain);
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
