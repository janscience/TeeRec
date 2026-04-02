#include <InputTDM.h>
#include <InputTDMSettings.h>

const char *InputTDMSettings::SourceStrings[MaxSource] = {
  "differential", "single ended", "digital" };

const InputTDMSettings::SOURCE InputTDMSettings::SourceEnums[MaxSource] = {
  DIFFERENTIAL, SINGLE_ENDED, DIGITAL };


InputTDMSettings::InputTDMSettings(Menu &menu, uint32_t rate,
				   int nchannels, float gain,
				   float pregain, SOURCE source) :
  InputSettings(menu, rate, pregain),
  NChannels(*this, "NChannels", nchannels, 1, 128, "%hu", 0, 0, Admin),
  Source(*this, "Source", source, SourceEnums, SourceStrings, MaxSource, Admin),
  Gain(*this, "Gain", gain, "%.1f", "dB") {
  disable("Source");
  move(&Source, 3);
  move(&PreGain, 3);
}


void InputTDMSettings::setNChannels(uint8_t nchannels) {
  NChannels.setValue(nchannels);
}


void InputTDMSettings::setSource(SOURCE source) {
  Source.setEnumValue(source);
}


void InputTDMSettings::setGainDecibel(float gain) {
  Gain.setValue(gain);
}

