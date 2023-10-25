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
  InputTDMSettings(uint32_t rate=0, int nchannels=16, float gain=0);

  // Constructor setting configuration name.
  InputTDMSettings(const char *name, uint32_t rate=0, int nchannels=16,
		   float gain=0);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate.value(); };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return number of channels.
  // The number of channels is set from the configuration file
  // but needs to be passed manually to appropriate Control instances.
  uint8_t nchannels() const { return NChannels.value(); };
  
  // Set the number of channels.
  void setNChannels(uint8_t nchannels);

  // Return gain in dB.
  // The gain is set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  float gain() const { return Gain.value(); };
  
  // Set gain in dB.
  // The gain is also set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  void setGain(float gain);

  // Apply TDM settings on tdm.
  void configure(InputTDM *tdm);

  // Transfer TDM settings from tdm to the InputTDMSettings instance.
  void setConfiguration(InputTDM *tdm);

    
protected:

  NumberParameter<uint32_t> Rate;
  NumberParameter<uint8_t> NChannels;
  NumberParameter<float> Gain;
  
};

#endif
