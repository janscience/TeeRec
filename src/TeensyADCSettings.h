/*
  TeensyADCSettings - configuration settings for TeensyADC.
  Created by Jan Benda, Dec 27th, 2022.
*/


#ifndef TeensyADCSettings_h
#define TeensyADCSettings_h


#include <Arduino.h>
#include <ADC.h>
#include <Configurable.h>


class TeensyADCSettings : public Configurable {

public:

  // Constructor. Sets configuration name to "ADC".
  TeensyADCSettings(uint32_t rate=0, uint8_t bits=16, uint8_t averaging=4,
		    ADC_CONVERSION_SPEED conversion_speed=ADC_CONVERSION_SPEED::HIGH_SPEED,
		    ADC_SAMPLING_SPEED sampling_speed=ADC_SAMPLING_SPEED::HIGH_SPEED,
		    ADC_REFERENCE reference=ADC_REFERENCE::REF_3V3);

  // Constructor. Provides an instance of a TeensyADC and
  // sets configuration name to "ADC".
  TeensyADCSettings(TeensyADC *adc, uint32_t rate=0, uint8_t bits=16, uint8_t averaging=4,
		    ADC_CONVERSION_SPEED conversion_speed=ADC_CONVERSION_SPEED::HIGH_SPEED,
		    ADC_SAMPLING_SPEED sampling_speed=ADC_SAMPLING_SPEED::HIGH_SPEED,
		    ADC_REFERENCE reference=ADC_REFERENCE::REF_3V3);

  // Constructor setting configuration name.
  TeensyADCSettings(const char *name, uint32_t rate=0,
		    uint8_t bits=16, uint8_t averaging=4,
		    ADC_CONVERSION_SPEED conversion_speed=ADC_CONVERSION_SPEED::HIGH_SPEED,
		    ADC_SAMPLING_SPEED sampling_speed=ADC_SAMPLING_SPEED::HIGH_SPEED,
		    ADC_REFERENCE reference=ADC_REFERENCE::REF_3V3);

  // Constructor. Provides an instance of a TeensyADC and
  // sets configuration name.
  TeensyADCSettings(TeensyADC *adc, const char *name, uint32_t rate=0,
		    uint8_t bits=16, uint8_t averaging=4,
		    ADC_CONVERSION_SPEED conversion_speed=ADC_CONVERSION_SPEED::HIGH_SPEED,
		    ADC_SAMPLING_SPEED sampling_speed=ADC_SAMPLING_SPEED::HIGH_SPEED,
		    ADC_REFERENCE reference=ADC_REFERENCE::REF_3V3);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate; };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return ADC resolution in bits per sample.
  uint8_t resolution() const { return Bits; };
  
  // Set resolution of data acquisition in bits per sample
  // Valid values are 10, 12, or 16 bit.
  void setResolution(uint8_t bits);

  // Return the number of averages taken by each sample.
  uint8_t averaging(void) const { return Averaging; };

  // Set the number of averages taken by each sample.
  // Valid values are 0, 4, 8, 16, or 32.
  void setAveraging(uint8_t num);

  // Set the conversion speed by changing the ADC clock.
  // One of VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS,
  // HIGH_SPEED, VERY_HIGH_SPEED, ADACK_2_4, ADACK_4_0, ADACK_5_2 or ADACK_6_2.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   Sampling: Load an internal capacitor with the voltage you want to measure.
  //   The longer you let this capacitor be charged, the closest it will resemble the voltage.
  //   Conversion: Convert that voltage into a digital representation that is as close
  //   as possible to the selected resolution.
  void setConversionSpeed(ADC_CONVERSION_SPEED speed);

  // The conversion speed.
  ADC_CONVERSION_SPEED conversionSpeed() const { return ConversionSpeed; };
  
  // Set the sampling speed.
  // Increase the sampling speed for low impedance sources, 
  // decrease it for higher impedance ones.
  // One of VERY_LOW_SPEED, LOW_SPEED, LOW_MED_SPEED, MED_SPEED, MED_HIGH_SPEED,
  // HIGH_SPEED, HIGH_VERY_HIGH_SPEED, VERY_HIGH_SPEED.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   Sampling: Load an internal capacitor with the voltage you want to measure.
  //   The longer you let this capacitor be charged, the closest it will resemble the voltage.
  //   Conversion: Convert that voltage into a digital representation that is as close
  //   as possible to the selected resolution.
  void setSamplingSpeed(ADC_SAMPLING_SPEED speed);

  // The sampling speed.
  ADC_SAMPLING_SPEED samplingSpeed() const { return SamplingSpeed; };

  // Set the voltage preference.
  // One of ADC_REFERENCE::REF_3V3 (default), ADC_REFERENCE::REF_1V2, or ADC_REFERENCE::REF_EXT
  // Teensy 4.x has only 3V3, on Teensy 3.x EXT equals 3V3.
  void setReference(ADC_REFERENCE ref);

  // The voltage reference.
  ADC_REFERENCE reference() const { return Reference; };
  
  // Configure ADC settings with the provided key-value pair.
  // If an TeensyADC instance was provided to the constructor,
  // the settings are also directly applied to this TeensyADC instance.
  virtual void configure(const char *key, const char *val);

  // Apply ADC settings on adc.
  // If no adc is provided, the one provided to the constructor is used.
  void configure(TeensyADC *adc=0);

  // Transfer ADC settings from adc to the TeensyADCSettings instance.
  void setConfiguration(TeensyADC *adc=0);
  
  // Report current settings on Serial.
  void report() const;

    
protected:

  uint32_t Rate;
  uint8_t Bits;
  uint8_t Averaging;
  ADC_CONVERSION_SPEED ConversionSpeed;
  ADC_SAMPLING_SPEED SamplingSpeed;
  ADC_REFERENCE Reference;
  TeensyADC *ADC;

};

#endif
