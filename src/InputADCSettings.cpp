#include <InputADC.h>
#include <InputADCSettings.h>


const uint8_t InputADCSettings::BitsSelection[InputADCSettings::NBitsSelection] = {10, 12, 16};
  
const uint8_t InputADCSettings::AveragingSelection[InputADCSettings::NAveragingSelection] = {0, 4, 8, 16, 32};


InputADCSettings::InputADCSettings(Menu &menu, uint32_t rate, uint8_t bits,
				   uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference, float pregain) :
  InputSettings(menu, rate, pregain),
  Bits(*this, "Resolution", bits, "%.0f", "bits", "",
       BitsSelection, NBitsSelection),
  Averaging(*this, "Averaging", averaging, "%hu", "", "",
	    AveragingSelection, NAveragingSelection),
  ConversionSpeed(*this, "Conversion", conversion_speed,
		  InputADC::ConversionEnums,
		  InputADC::ConversionShortStrings,
		  InputADC::MaxConversions),
  SamplingSpeed(*this, "Sampling", sampling_speed,
		InputADC::SamplingEnums,
		InputADC::SamplingShortStrings,
		InputADC::MaxSamplings),
  Reference(*this, "Reference", reference,
	    InputADC::ReferenceEnums,
	    InputADC::ReferenceStrings,
	    InputADC::MaxReferences) {
  move(&PreGain, 6);
}


void InputADCSettings::setResolution(uint8_t bits) {
  Bits.setValue(bits);
}


void InputADCSettings::setAveraging(uint8_t num) {
  Averaging.setValue(num);
}


void InputADCSettings::setConversionSpeed(ADC_CONVERSION_SPEED speed) {
  ConversionSpeed.setValue(speed);
}


void InputADCSettings::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed.setValue(speed);
}


void InputADCSettings::setReference(ADC_REFERENCE ref) {
  Reference.setValue(ref);
}


void InputADCSettings::configure(Input *input) const {
  InputADC *adc = static_cast<InputADC *>(input);
  if (adc == 0)
    return;
  adc->setRate(rate());
  adc->setResolution(resolution());
  adc->setAveraging(averaging());
  adc->setConversionSpeed(conversionSpeed());
  adc->setSamplingSpeed(samplingSpeed());
  adc->setReference(reference());
}


void InputADCSettings::setConfiguration(const Input *input) {
  const InputADC *adc = static_cast<const InputADC *>(input);
  if (adc == 0)
    return;
  setRate(adc->rate());
  setResolution(adc->resolution());
  setAveraging(adc->averaging());
  setConversionSpeed(adc->conversionSpeed());
  setSamplingSpeed(adc->samplingSpeed());
  setReference(adc->reference());
}

