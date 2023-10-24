#include <InputADC.h>
#include <InputADCSettings.h>


ConversionSpeedParameter::ConversionSpeedParameter(Configurable *cfg,
						   const char *key,
						   ADC_CONVERSION_SPEED speed) :
  Parameter(cfg, "Conversion"),
  Value(speed) {
}


void ConversionSpeedParameter::parseValue(const char *val) {
  Value = InputADC::conversionSpeedEnum(val);
}


void ConversionSpeedParameter::valueStr(char *str) {
  strcpy(str, InputADC::conversionSpeedStr(Value));
}


SamplingSpeedParameter::SamplingSpeedParameter(Configurable *cfg,
					       const char *key,
					       ADC_SAMPLING_SPEED speed) :
  Parameter(cfg, "Sampling"),
  Value(speed) {
}


void SamplingSpeedParameter::parseValue(const char *val) {
  Value = InputADC::samplingSpeedEnum(val);
}


void SamplingSpeedParameter::valueStr(char *str) {
  strcpy(str, InputADC::samplingSpeedStr(Value));
}


ReferenceParameter::ReferenceParameter(Configurable *cfg,
				       const char *key,
				       ADC_REFERENCE reference) :
  Parameter(cfg, "Reference"),
  Value(reference) {
}


void ReferenceParameter::parseValue(const char *val) {
  Value = InputADC::referenceEnum(val);
}


void ReferenceParameter::valueStr(char *str) {
  strcpy(str, InputADC::referenceStr(Value));
}


InputADCSettings::InputADCSettings(uint32_t rate, uint8_t bits,
				   uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable("ADC"),
  Rate(rate),
  RateP(this, "SamplingRate", &Rate, "%lu"),
  Bits(this, "Resolution", bits, "%hu", "bits"),
  Averaging(this, "Averaging", averaging, "%hu"),
  ConversionSpeed(this, "Conversion", conversion_speed),
  SamplingSpeed(this, "Sampling", sampling_speed),
  Reference(this, "Reference", reference) {
}


InputADCSettings::InputADCSettings(const char *name, uint32_t rate,
				   uint8_t bits, uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable(name),
  Rate(rate),
  RateP(this, "SamplingRate", &Rate, "%.0f"),
  Bits(this, "Resolution", bits, "%hu", "bits"),
  Averaging(this, "Averaging", averaging, "%hu"),
  ConversionSpeed(this, "Conversion", conversion_speed),
  SamplingSpeed(this, "Sampling", sampling_speed),
  Reference(this, "Reference", reference) {
}


void InputADCSettings::setRate(uint32_t rate) {
  Rate = rate;
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

