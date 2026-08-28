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
		Input::SOURCE source=Input::SINGLE_ENDED,
		size_t nsource=Input::MaxSource,
		float highpass=0.0);
  
  // Constructor. Sets configuration name.
  InputSettings(const char *name, uint32_t rate=0, float pregain=1,
		Input::SOURCE source=Input::SINGLE_ENDED,
		float highpass=0.0);

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

  // Set format string for displaying pregain.
  void setPreGainFormat(const char *format);

  // Provide a list of valid pregain.
  void setPreGainSelection(const float *selection, size_t n);

  // Return channel source.
  Input::SOURCE source() const { return Source.enumValue(); };
  
  // Set the channel source.
  void setSource(Input::SOURCE source);
  
  // Set maximum number of channel source selections.
  void setSourceNSelection(size_t n);

  // Return cutoff frequency of high-pass filter in Hertz.
  float highpass() const { return Highpass.value(); };
  
  // Set cutoff frequency of highpass filter in Hertz.
  void setHighpass(float cutoff);

  // Provide a list of valid cutoff frequencies for the highpass filter in Hertz.
  void setHighpassSelection(const float *selection, size_t n);

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
  NumberParameter<float> Highpass;
  
};

#endif
