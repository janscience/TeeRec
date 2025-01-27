#include <Input.h>
#include <InputSettings.h>


InputSettings::InputSettings(uint32_t rate, float pregain) :
  InputSettings("ADC", rate, pregain) {
}


InputSettings::InputSettings(const char *name, uint32_t rate, float pregain) :
  Configurable(name),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f") {
  PreGain.disable();
}


void InputSettings::setRate(uint32_t rate) {
  Rate.setValue(rate);
}


void InputSettings::setRateSelection(uint32_t *selection, size_t n) {
  Rate.setSelection(selection, n);
}


void InputSettings::setPreGain(float pregain) {
  PreGain.setValue(pregain);
}


void InputSettings::configure(Input *input) {
  if (input == 0)
    return;
  input->setRate(rate());
}


void InputSettings::setConfiguration(Input *input) {
  if (input == 0)
    return;
  setRate(input->rate());
}
