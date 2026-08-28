#include <InputSettings.h>


InputSettings::InputSettings(Menu &menu, uint32_t rate, float pregain,
			     Input::SOURCE source, size_t nsource,
			     float highpass) :
  Menu(menu, "ADC"),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  Source(*this, "Source", source, Input::SourceEnums, Input::SourceStrings,
	 nsource, Admin),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f", 0, 0, Admin),
  Highpass(*this, "Highpass", highpass, 0.0, 100000.0, "%.1f", "Hz", "Hz") {
  Source.disable();
  PreGain.disable();
  Highpass.disable();
}


InputSettings::InputSettings(const char *name, uint32_t rate, float pregain,
			     Input::SOURCE source, float highpass) :
  Menu(name),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  Source(*this, "Source", source, Input::SourceEnums, Input::SourceStrings,
	 Input::MaxSource, Admin),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f", 0, 0, Admin),
  Highpass(*this, "Highpass", highpass, 0.0, 100000.0, "%.1f", "Hz", "Hz") {
  Source.disable();
  PreGain.disable();
  Highpass.disable();
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


void InputSettings::setPreGainFormat(const char *format) {
  PreGain.setFormat(format);
}


void InputSettings::setPreGainSelection(const float *selection, size_t n) {
  PreGain.setSelection(selection, n);
}


void InputSettings::setSource(Input::SOURCE source) {
  Source.setEnumValue(source);
}


void InputSettings::setSourceNSelection(size_t nsource) {
  Source.setNSelection(nsource);
}


void InputSettings::setHighpass(float cutoff) {
  Highpass.setValue(cutoff);
}


void InputSettings::setHighpassSelection(const float *selection, size_t n) {
  Highpass.setSelection(selection, n);
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
