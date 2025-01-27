#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(uint32_t rate, int nchannels,
				   float gain, float pregain) :
  InputTDMSettings("ADC", rate, nchannels, gain, pregain) {
}


InputTDMSettings::InputTDMSettings(const char *name, uint32_t rate,
				   int nchannels,
				   float gain, float pregain) :
  InputSettings(name, rate, pregain),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu"),
  Gain(*this, "Gain", gain, "%.1f", "dB") {
  move(&PreGain, 3);
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setGain(float gain) {
  Gain.setValue(gain);
}

