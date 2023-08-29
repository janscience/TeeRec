#include <InputADC.h>
#include <InputADCSettings.h>


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
  ADC(0) {
}


InputADCSettings::InputADCSettings(InputADC *adc, uint32_t rate,
				   uint8_t bits, uint8_t averaging,
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
  ADC(adc) {
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
  ADC(0) {
}


InputADCSettings::InputADCSettings(InputADC *adc, const char *name,
				   uint32_t rate, uint8_t bits,
				   uint8_t averaging,
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
  ADC(adc) {
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


void InputADCSettings::configure(const char *key, const char *val) {
  char pval[30];
  if (strcmp(key, "samplingrate") == 0) {
    setRate(uint32_t(parseFrequency(val)));
    sprintf(pval, "%luHz", Rate);
    if (ADC != 0)
      ADC->setRate(rate());
  }
  else if (strcmp(key, "resolution") == 0) {
    setResolution(atoi(val));
    sprintf(pval, "%hubits", Bits);
    if (ADC != 0)
      ADC->setResolution(resolution());
  }
  else if (strcmp(key, "averaging") == 0) {
    setAveraging(atoi(val));
    sprintf(pval, "%hu", Averaging);
    if (ADC != 0)
      ADC->setAveraging(averaging());
  }
  else if (strcmp(key, "conversion") == 0) {
    setConversionSpeed(InputADC::conversionSpeedEnum(val));
    strcpy(pval, InputADC::conversionSpeedStr(ConversionSpeed));
    if (ADC != 0)
      ADC->setConversionSpeed(conversionSpeed());
  }
  else if (strcmp(key, "sampling") == 0) {
    setSamplingSpeed(InputADC::samplingSpeedEnum(val));
    strcpy(pval, InputADC::samplingSpeedStr(SamplingSpeed));
    if (ADC != 0)
      ADC->setSamplingSpeed(samplingSpeed());
  }
  else if (strcmp(key, "reference") == 0) {
    setReference(InputADC::referenceEnum(val));
    strcpy(pval, InputADC::referenceStr(Reference));
    if (ADC != 0)
      ADC->setReference(reference());
  }
  else {
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
    return;
  }
  Serial.printf("  set %s-%s to %s\n", name(), key, pval);
}


void InputADCSettings::configure(InputADC *adc) {
  if (adc == 0)
    adc = ADC;
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
    adc = ADC;
  if (adc == 0)
    return;
  setRate(adc->rate());
  setResolution(adc->resolution());
  setAveraging(adc->averaging());
  setConversionSpeed(adc->conversionSpeed());
  setSamplingSpeed(adc->samplingSpeed());
  setReference(adc->reference());
}


void InputADCSettings::report() const {
  Serial.printf("%s settings:\n", name());
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  averaging:  %d\n", Averaging);
  Serial.printf("  conversion: %s\n", InputADC::conversionSpeedStr(ConversionSpeed));
  Serial.printf("  sampling:   %s\n", InputADC::samplingSpeedStr(SamplingSpeed));
  Serial.printf("  reference:  %s\n", InputADC::referenceStr(Reference));
}
