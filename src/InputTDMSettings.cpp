#include <InputTDMSettings.h>


InputTDMSettings::InputTDMSettings(Menu &menu, uint32_t rate,
				   int nchannels, float gain,
				   float pregain, Input::SOURCE source,
				   size_t nsource, bool reverse) :
  InputSettings(menu, rate, pregain, source, nsource),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu", 0, 0, Admin),
  Gain(*this, "Gain", gain, "%.1f", "dB"),
  ReverseInputs(*this, "ReverseInputs", reverse, Admin) {
  move(&PreGain, 3);
  ReverseInputs.disable();
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setGainDecibel(float gain) {
  Gain.setValue(gain);
}


void InputTDMSettings::setReverseInputs(bool reverse) {
  ReverseInputs.setBoolValue(reverse);
}

