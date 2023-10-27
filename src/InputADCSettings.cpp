#include <InputADC.h>
#include <InputADCSettings.h>


const uint8_t InputADCSettings::BitsSelection[InputADCSettings::NBitsSelection] = {10, 12, 16};
  
const uint8_t InputADCSettings::AveragingSelection[InputADCSettings::NAveragingSelection] = {0, 4, 8, 16, 32};


ConversionSpeedParameter::ConversionSpeedParameter(Configurable *cfg,
						   const char *key,
						   ADC_CONVERSION_SPEED speed) :
  Parameter(cfg, "Conversion"),
  Value(speed) {
}


bool ConversionSpeedParameter::parseValue(const char *val) {
  Value = InputADC::conversionSpeedEnum(val);
  return true;
}


void ConversionSpeedParameter::valueStr(char *str) const {
  strcpy(str, InputADC::conversionSpeedStr(Value));
}


SamplingSpeedParameter::SamplingSpeedParameter(Configurable *cfg,
					       const char *key,
					       ADC_SAMPLING_SPEED speed) :
  Parameter(cfg, "Sampling"),
  Value(speed) {
}


bool SamplingSpeedParameter::parseValue(const char *val) {
  Value = InputADC::samplingSpeedEnum(val);
  return true;
}


void SamplingSpeedParameter::valueStr(char *str) const {
  strcpy(str, InputADC::samplingSpeedStr(Value));
}


ReferenceParameter::ReferenceParameter(Configurable *cfg,
				       const char *key,
				       ADC_REFERENCE reference) :
  Parameter(cfg, "Reference"),
  Value(reference) {
}


bool ReferenceParameter::parseValue(const char *val) {
  Value = InputADC::referenceEnum(val);
  return true;
}


void ReferenceParameter::valueStr(char *str) const {
  strcpy(str, InputADC::referenceStr(Value));
}


InputADCSettings::InputADCSettings(uint32_t rate, uint8_t bits,
				   uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable("ADC"),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  Bits(this, "Resolution", bits, "%.0f", "bits"),
  Averaging(this, "Averaging", averaging, "%hu"),
  ConversionSpeed(this, "Conversion", conversion_speed),
  SamplingSpeed(this, "Sampling", sampling_speed),
  Reference(this, "Reference", reference) {
  Bits.setSelection(BitsSelection, NBitsSelection);
  Averaging.setSelection(AveragingSelection, NAveragingSelection);
}


InputADCSettings::InputADCSettings(const char *name, uint32_t rate,
				   uint8_t bits, uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable(name),
  Rate(this, "SamplingRate", rate, "%.1f", "Hz", "kHz"),
  Bits(this, "Resolution", bits, "%.0f", "bits"),
  Averaging(this, "Averaging", averaging, "%hu"),
  ConversionSpeed(this, "Conversion", conversion_speed),
  SamplingSpeed(this, "Sampling", sampling_speed),
  Reference(this, "Reference", reference) {
  Bits.setSelection(BitsSelection, NBitsSelection);
  Averaging.setSelection(AveragingSelection, NAveragingSelection);
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
  ConversionSpeed.Value = speed;
}


void InputADCSettings::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed.Value = speed;
}


void InputADCSettings::setReference(ADC_REFERENCE ref) {
  Reference.Value = ref;
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

