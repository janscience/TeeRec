#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(uint32_t rate, int nchannels,
				   bool exactchannels,
				   float gain, float pregain) :
  Configurable("ADC"),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu"),
  ExactChannels(*this, "ExactChannels", exactchannels),
  Gain(*this, "Gain", gain, "%.1f", "dB"),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f") {
  PreGain.disable();
}


InputTDMSettings::InputTDMSettings(const char *name, uint32_t rate,
				   int nchannels, bool exactchannels,
				   float gain, float pregain) :
  Configurable(name),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu"),
  ExactChannels(*this, "ExactChannels", exactchannels),
  Gain(*this, "Gain", gain, "%.1f", "dB"),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f") {
  PreGain.disable();
}


void InputTDMSettings::setRate(uint32_t rate) {
  Rate.setValue(rate);
}


void InputTDMSettings::setRateSelection(uint32_t *selection, size_t n) {
  Rate.setSelection(selection, n);
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setExactChannels(bool exact) {
  ExactChannels.setValue(exact);
}


void InputTDMSettings::setGain(float gain) {
  Gain.setValue(gain);
}


void InputTDMSettings::setPreGain(float pregain) {
  PreGain.setValue(pregain);
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
