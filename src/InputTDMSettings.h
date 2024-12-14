/*
  InputTDMSettings - configuration settings for InputTDM.
  Created by Jan Benda, Aug 19, 2023.
*/


#ifndef InputTDMSettings_h
#define InputTDMSettings_h


#include <Configurable.h>
#include <Parameter.h>


class InputTDM;


class InputTDMSettings : public Configurable {

public:

  // Constructor. Sets configuration name to "ADC".
  InputTDMSettings(uint32_t rate=0, int nchannels=16,
		   bool exactchannels=false, float gain=0, float pregain=1);

  // Constructor setting configuration name.
  InputTDMSettings(const char *name, uint32_t rate=0, int nchannels=16,
		   bool exactchannels=false, float gain=0, float pregain=1);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate.value(); };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Provide a list of valid sampling rates in Hz.
  void setRateSelection(uint32_t *selection, size_t n);

  // Return number of channels.
  // The number of channels is set from the configuration file
  // but needs to be passed manually to appropriate Control instances.
  uint8_t nchannels() const { return NChannels.value(); };
  
  // Set the number of channels.
  void setNChannels(uint8_t nchannels);

  // If true, the logger should halt, if the number of available
  // channels does not match the number of requested channels.
  bool exactChannels() const { return ExactChannels.value(); };
  
  // Set whether the number of available channels must match the
  // number of requested channels.
  void setExactChannels(bool exact);

  // Return gain in dB.
  // The gain is set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  float gain() const { return Gain.value(); };
  
  // Set gain in dB.
  // The gain is also set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  void setGain(float gain);

  // Return pregain as a factor.
  // This is the fixed gain of a first amplification stage.
  float pregain() const { return PreGain.value(); };
  
  // Set pregain as a factor.
  // This is the fixed gain of a first amplification stage.
  void setPreGain(float pregain);

  // Apply TDM settings on tdm.
  void configure(InputTDM *tdm);

  // Transfer TDM settings from tdm to the InputTDMSettings instance.
  void setConfiguration(InputTDM *tdm);

    
protected:

  NumberParameter<uint32_t> Rate;
  NumberParameter<uint8_t> NChannels;
  BoolParameter ExactChannels;
  NumberParameter<float> Gain;
  NumberParameter<float> PreGain;
  
};

#endif
