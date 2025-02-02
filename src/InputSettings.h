/*
  InputSettings - configuration settings for any Input class.
  Created by Jan Benda, Jan 27, 2025.
*/


#ifndef InputSettings_h
#define InputSettings_h


#include <Configurable.h>
#include <Parameter.h>


class Input;


class InputSettings : public Configurable {

public:

  // Constructor. Sets configuration name to "ADC".
  InputSettings(uint32_t rate=0, float pregain=1);

  // Constructor setting configuration name.
  InputSettings(const char *name, uint32_t rate=0, float pregain=1);

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

  // Apply settings on input.
  // Default implementation sets the sampling rate.
  virtual void configure(Input *input);

  // Transfer settings from input to this InputSettings instance.
  // Default implementation transfers the sampling rate.
  virtual void setConfiguration(Input *input);

    
protected:

  NumberParameter<uint32_t> Rate;
  NumberParameter<float> PreGain;
  
};

#endif
