#include <TeensyADC.h>
#include <TeensyADCSettings.h>


TeensyADCSettings::TeensyADCSettings(uint32_t rate, uint8_t bits,
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


TeensyADCSettings::TeensyADCSettings(TeensyADC *adc, uint32_t rate,
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


TeensyADCSettings::TeensyADCSettings(const char *name, uint32_t rate,
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


TeensyADCSettings::TeensyADCSettings(TeensyADC *adc, const char *name,
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


void TeensyADCSettings::setRate(uint32_t rate) {
  Rate = rate;
}


void TeensyADCSettings::setResolution(uint8_t bits) {
  Bits = bits;
}


void TeensyADCSettings::setAveraging(uint8_t num) {
  Averaging = num;
}


void TeensyADCSettings::setConversionSpeed(ADC_CONVERSION_SPEED speed) {
  ConversionSpeed = speed;
}


void TeensyADCSettings::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed = speed;
}


void TeensyADCSettings::setReference(ADC_REFERENCE ref) {
  Reference = ref;
}


void TeensyADCSettings::configure(const char *key, const char *val) {
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
    setConversionSpeed(TeensyADC::conversionSpeedEnum(val));
    strcpy(pval, TeensyADC::conversionSpeedStr(ConversionSpeed));
    if (ADC != 0)
      ADC->setConversionSpeed(conversionSpeed());
  }
  else if (strcmp(key, "sampling") == 0) {
    setSamplingSpeed(TeensyADC::samplingSpeedEnum(val));
    strcpy(pval, TeensyADC::samplingSpeedStr(SamplingSpeed));
    if (ADC != 0)
      ADC->setSamplingSpeed(samplingSpeed());
  }
  else if (strcmp(key, "reference") == 0) {
    setReference(TeensyADC::referenceEnum(val));
    strcpy(pval, TeensyADC::referenceStr(Reference));
    if (ADC != 0)
      ADC->setReference(reference());
  }
  else {
    Serial.printf("  %s key \"%s\" not found.\n", name(), key);
    return;
  }
  Serial.printf("  set %s-%s to %s\n", name(), key, pval);
}


void TeensyADCSettings::configure(TeensyADC *adc) {
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


void TeensyADCSettings::setConfiguration(TeensyADC *adc) {
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


void TeensyADCSettings::report() const {
  Serial.printf("%s settings:\n", name());
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  averaging:  %d\n", Averaging);
  Serial.printf("  conversion: %s\n", TeensyADC::conversionSpeedStr(ConversionSpeed));
  Serial.printf("  sampling:   %s\n", TeensyADC::samplingSpeedStr(SamplingSpeed));
  Serial.printf("  reference:  %s\n", TeensyADC::referenceStr(Reference));
}
