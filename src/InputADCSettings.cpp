#include <InputADC.h>
#include <InputADCSettings.h>


const uint8_t InputADCSettings::BitsSelection[InputADCSettings::NBitsSelection] = {10, 12, 16};
  
const uint8_t InputADCSettings::AveragingSelection[InputADCSettings::NAveragingSelection] = {0, 4, 8, 16, 32};


InputADCSettings::InputADCSettings(uint32_t rate, uint8_t bits,
				   uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable("ADC"),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  Bits(this, "Resolution", bits, "%.0f", "bits", "",
       BitsSelection, NBitsSelection),
  Averaging(this, "Averaging", averaging, "%hu", "", "",
	    AveragingSelection, NAveragingSelection),
  ConversionSpeed(this, "Conversion", conversion_speed,
		  InputADC::ConversionEnums,
		  InputADC::ConversionShortStrings,
		  InputADC::MaxConversions),
  SamplingSpeed(this, "Sampling", sampling_speed,
		InputADC::SamplingEnums,
		InputADC::SamplingShortStrings,
		InputADC::MaxSamplings),
  Reference(this, "Reference", reference,
	    InputADC::ReferenceEnums,
	    InputADC::ReferenceStrings,
	    InputADC::MaxReferences) {
}


InputADCSettings::InputADCSettings(const char *name, uint32_t rate,
				   uint8_t bits, uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable(name),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  Bits(this, "Resolution", bits, "%.0f", "bits", "",
       BitsSelection, NBitsSelection),
  Averaging(this, "Averaging", averaging, "%hu", "", "",
	    AveragingSelection, NAveragingSelection),
  ConversionSpeed(this, "Conversion", conversion_speed,
		  InputADC::ConversionEnums,
		  InputADC::ConversionShortStrings,
		  InputADC::MaxConversions),
  SamplingSpeed(this, "Sampling", sampling_speed,
		InputADC::SamplingEnums,
		InputADC::SamplingShortStrings,
		InputADC::MaxSamplings),
  Reference(this, "Reference", reference,
	    InputADC::ReferenceEnums,
	    InputADC::ReferenceStrings,
	    InputADC::MaxReferences) {
}


void InputADCSettings::setRate(uint32_t rate) {
  Rate.setValue(rate);
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


void InputADCSettings::configure(InputADC *adc) {
  if (adc == 0)
    return;
  adc->setRate(rate());
  adc->setResolution(resolution());
  adc->setAveraging(averaging());
  adc->setConversionSpeed(conversionSpeed());
  adc->setSamplingSpeed(samplingSpeed());
  adc->setReference(reference());
}


void InputADCSettings::setConfiguration(InputADC *adc) {
  if (adc == 0)
    return;
  setRate(adc->rate());
  setResolution(adc->resolution());
  setAveraging(adc->averaging());
  setConversionSpeed(adc->conversionSpeed());
  setSamplingSpeed(adc->samplingSpeed());
  setReference(adc->reference());
}

