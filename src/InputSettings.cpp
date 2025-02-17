#include <Input.h>
#include <InputSettings.h>


InputSettings::InputSettings(Menu &menu, uint32_t rate, float pregain) :
  Menu(menu, "ADC"),
  Rate(*this, "SamplingRate", rate, 1, 1000000, "%.1f", "Hz", "kHz"),
  PreGain(*this, "Pregain", pregain, 0, 100000, "%.1f") {
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


void InputSettings::configure(Input *input) const {
  if (input == 0)
    return;
  input->setRate(rate());
  input->setPreGain(pregain());
}


void InputSettings::setConfiguration(const Input *input) {
  if (input == 0)
    return;
  setRate(input->rate());
  setPreGain(input->pregain());
}
