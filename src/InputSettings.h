/*
  InputSettings - configuration settings for any Input class.
  Created by Jan Benda, Jan 27, 2025.
*/


#ifndef InputSettings_h
#define InputSettings_h


#include <MicroConfig.h>
#include <Input.h>


class InputSettings : public Menu {

public:

  // Constructor. Sets configuration name to "ADC".
  InputSettings(Menu &menu, uint32_t rate=0, float pregain=1,
		Input::SOURCE source=Input::SINGLE_ENDED);
  
  // Constructor. Sets configuration name.
  InputSettings(const char *name, uint32_t rate=0, float pregain=1,
		Input::SOURCE source=Input::SINGLE_ENDED);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate.value(); };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Provide a list of valid sampling rates in Hz.
  void setRateSelection(const uint32_t *selection, size_t n);

  // Return pregain as a factor.
  // This is the fixed gain of a first amplification stage.
  float pregain() const { return PreGain.value(); };
  
  // Set pregain as a factor.
  // This is the fixed gain of a first amplification stage.
  void setPreGain(float pregain);

  // Return channel source.
  Input::SOURCE source() const { return Source.enumValue(); };
  
  // Set the channel source.
  void setSource(Input::SOURCE source);

  // Apply settings on input.
  // Default implementation sets the sampling rate.
  virtual void configure(Input *input) const;

  // Transfer settings from input to this InputSettings instance.
  // Default implementation transfers the sampling rate.
  virtual void setConfiguration(const Input *input);

    
protected:

  NumberParameter<uint32_t> Rate;
  EnumParameter<Input::SOURCE> Source;
  NumberParameter<float> PreGain;
  
};

#endif
