#include <InputTDM.h>
#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(Menu &menu, uint32_t rate,
				   int nchannels,
				   float gain, float pregain) :
  InputSettings(menu, rate, pregain),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu", 0, 0, Admin),
  Gain(*this, "Gain", gain, "%.1f", "dB") {
  move(&PreGain, 3);
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setGainDecibel(float gain) {
  Gain.setValue(gain);
}

