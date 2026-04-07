#include <InputSettings.h>


InputSettings::InputSettings(Menu &menu, uint32_t rate, float pregain,
			     Input::SOURCE source) :
  Menu(menu, "ADC"),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  Source(*this, "Source", source, Input::SourceEnums, Input::SourceStrings,
	 Input::MaxSource, Admin),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f", 0, 0, Admin) {
  Source.disable();
  PreGain.disable();
}


InputSettings::InputSettings(const char *name, uint32_t rate, float pregain,
			     Input::SOURCE source) :
  Menu(name),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  Source(*this, "Source", source, Input::SourceEnums, Input::SourceStrings,
	 Input::MaxSource, Admin),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f", 0, 0, Admin) {
  Source.disable();
  PreGain.disable();
}


void InputSettings::setRate(uint32_t rate) {
  Rate.setValue(rate);
}


void InputSettings::setRateSelection(const uint32_t *selection, size_t n) {
  Rate.setSelection(selection, n);
}


void InputSettings::setPreGain(float pregain) {
  PreGain.setValue(pregain);
}


void InputSettings::setSource(Input::SOURCE source) {
  Source.setEnumValue(source);
}


void InputSettings::configure(Input *input) const {
  if (input == 0)
    return;
  input->setRate(rate());
  input->setSource(source());
  input->setPreGain(pregain());
}


void InputSettings::setConfiguration(const Input *input) {
  if (input == 0)
    return;
  setRate(input->rate());
  setSource(input->source());
  setPreGain(input->pregain());
}
