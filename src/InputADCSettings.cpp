#include <InputADC.h>
#include <InputADCSettings.h>


ConversionSpeedParameter::ConversionSpeedParameter(Configurable *cfg,
						   const char *key,
						   ADC_CONVERSION_SPEED *speed) :
  Parameter(cfg, "Conversion"),
  Speed(speed) {
}


void ConversionSpeedParameter::parseValue(const char *val) {
  *Speed = InputADC::conversionSpeedEnum(val);
}


void ConversionSpeedParameter::valueStr(char *str) {
  strcpy(str, InputADC::conversionSpeedStr(*Speed));
}


SamplingSpeedParameter::SamplingSpeedParameter(Configurable *cfg,
					       const char *key,
					       ADC_SAMPLING_SPEED *speed) :
  Parameter(cfg, "Sampling"),
  Speed(speed) {
}


void SamplingSpeedParameter::parseValue(const char *val) {
  *Speed = InputADC::samplingSpeedEnum(val);
}


void SamplingSpeedParameter::valueStr(char *str) {
  strcpy(str, InputADC::samplingSpeedStr(*Speed));
}


ReferenceParameter::ReferenceParameter(Configurable *cfg,
				       const char *key,
				       ADC_REFERENCE *reference) :
  Parameter(cfg, "Reference"),
  Reference(reference) {
}


void ReferenceParameter::parseValue(const char *val) {
  *Reference = InputADC::referenceEnum(val);
}


void ReferenceParameter::valueStr(char *str) {
  strcpy(str, InputADC::referenceStr(*Reference));
}


InputADCSettings::InputADCSettings(uint32_t rate, uint8_t bits,
				   uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable("ADC"),
  Rate(rate),
  Bits(bits),
  Averaging(averaging),
  ConversionSpeed(conversion_speed),
  SamplingSpeed(sampling_speed),
  Reference(reference),
  RateP(this, "SamplingRate", &Rate, "%lu"),
  BitsP(this, "Resolution", &Bits, "%hu", "bits"),
  AveragingP(this, "Averaging", &Averaging, "%hu"),
  ConversionSpeedP(this, "Conversion", &ConversionSpeed),
  SamplingSpeedP(this, "Sampling", &SamplingSpeed),
  ReferenceP(this, "Reference", &Reference) {
}


InputADCSettings::InputADCSettings(const char *name, uint32_t rate,
				   uint8_t bits, uint8_t averaging,
				   ADC_CONVERSION_SPEED conversion_speed,
				   ADC_SAMPLING_SPEED sampling_speed,
				   ADC_REFERENCE reference) :
  Configurable(name),
  Rate(rate),
  Bits(bits),
  Averaging(averaging),
  ConversionSpeed(conversion_speed),
  SamplingSpeed(sampling_speed),
  Reference(reference),
  RateP(this, "SamplingRate", &Rate, "%.0f"),
  BitsP(this, "Resolution", &Bits, "%hu", "bits"),
  AveragingP(this, "Averaging", &Averaging, "%hu"),
  ConversionSpeedP(this, "Conversion", &ConversionSpeed),
  SamplingSpeedP(this, "Sampling", &SamplingSpeed),
  ReferenceP(this, "Reference", &Reference) {
}


void InputADCSettings::setRate(uint32_t rate) {
  Rate = rate;
}


void InputADCSettings::setResolution(uint8_t bits) {
  Bits = bits;
}


void InputADCSettings::setAveraging(uint8_t num) {
  Averaging = num;
}


void InputADCSettings::setConversionSpeed(ADC_CONVERSION_SPEED speed) {
  ConversionSpeed = speed;
}


void InputADCSettings::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed = speed;
}


void InputADCSettings::setReference(ADC_REFERENCE ref) {
  Reference = ref;
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

