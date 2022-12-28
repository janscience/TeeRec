#include <TeensyADC.h>
#include <TeensyADCSettings.h>


TeensyADCSettings::TeensyADCSettings() :
  Configurable("ADC"),
  Rate(0),
  Bits(16),
  Averaging(1),
  ConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED),
  SamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED),
  Reference(ADC_REFERENCE::REF_3V3) {
}


TeensyADCSettings::TeensyADCSettings(const char *name) :
  TeensyADCSettings() {
  setName(name);
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
  }
  else if (strcmp(key, "resolution") == 0) {
    setResolution(atoi(val));
    sprintf(pval, "%hubits", Bits);
  }
  else if (strcmp(key, "averaging") == 0) {
    setAveraging(atoi(val));
    sprintf(pval, "%hu", Averaging);
  }
  else if (strcmp(key, "conversion") == 0) {
    setConversionSpeed(TeensyADC::conversionSpeedEnum(val));
    strcpy(pval, TeensyADC::conversionSpeedStr(ConversionSpeed));
  }
  else if (strcmp(key, "sampling") == 0) {
    setSamplingSpeed(TeensyADC::samplingSpeedEnum(val));
    strcpy(pval, TeensyADC::samplingSpeedStr(SamplingSpeed));
  }
  else if (strcmp(key, "reference") == 0) {
    setReference(TeensyADC::referenceEnum(val));
    strcpy(pval, TeensyADC::referenceStr(Reference));
  }
  else {
    Serial.printf("  ADC key \"%s\" not found.\n", key);
    return;
  }
  Serial.printf("  set ADC-%s to %s\n", key, pval);
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
