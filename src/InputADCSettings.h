/*
  InputADCSettings - configuration settings for InputADC.
  Created by Jan Benda, Dec 27th, 2022.
*/


#ifndef InputADCSettings_h
#define InputADCSettings_h


#include <ADC.h>
#include <InputSettings.h>


class InputADCSettings : public InputSettings {

public:

  // Constructor. Sets configuration name to "ADC".
  InputADCSettings(Menu &menu, uint32_t rate=0, uint8_t bits=16, uint8_t averaging=4,
		   ADC_CONVERSION_SPEED conversion_speed=ADC_CONVERSION_SPEED::HIGH_SPEED,
		   ADC_SAMPLING_SPEED sampling_speed=ADC_SAMPLING_SPEED::HIGH_SPEED,
		   ADC_REFERENCE reference=ADC_REFERENCE::REF_3V3,
		   float pregain=1);

  // Return ADC resolution in bits per sample.
  uint8_t resolution() const { return Bits.value(); };
  
  // Set resolution of data acquisition in bits per sample
  // Valid values are 10, 12, or 16 bit.
  void setResolution(uint8_t bits);

  // Return the number of averages taken by each sample.
  uint8_t averaging(void) const { return Averaging.value(); };

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
  ADC_CONVERSION_SPEED conversionSpeed() const { return ConversionSpeed.value(); };
  
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
  ADC_SAMPLING_SPEED samplingSpeed() const { return SamplingSpeed.value(); };

  // Set the voltage preference.
  // One of ADC_REFERENCE::REF_3V3 (default), ADC_REFERENCE::REF_1V2, or ADC_REFERENCE::REF_EXT
  // Teensy 4.x has only 3V3, on Teensy 3.x EXT equals 3V3.
  void setReference(ADC_REFERENCE ref);

  // The voltage reference.
  ADC_REFERENCE reference() const { return Reference.value(); };

  // Apply settings on input.
  virtual void configure(Input *input);

  // Transfer settings from input to this InputADCSettings instance.
  virtual void setConfiguration(Input *input);

    
protected:

  NumberParameter<uint8_t> Bits;
  NumberParameter<uint8_t> Averaging;
  EnumParameter<ADC_CONVERSION_SPEED> ConversionSpeed;
  EnumParameter<ADC_SAMPLING_SPEED> SamplingSpeed;
  EnumParameter<ADC_REFERENCE> Reference;

  static const size_t NBitsSelection = 3;
  static const uint8_t BitsSelection[NBitsSelection];
  
  static const size_t NAveragingSelection = 5;
  static const uint8_t AveragingSelection[NAveragingSelection];

};

#endif
