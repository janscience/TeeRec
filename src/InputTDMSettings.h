/*
  InputTDMSettings - configuration settings for InputTDM.
  Created by Jan Benda, Aug 19, 2023.
*/


#ifndef InputTDMSettings_h
#define InputTDMSettings_h


#include <InputSettings.h>


class InputTDMSettings : public InputSettings {

public:

  // Constructor. Sets configuration name to "ADC".
  InputTDMSettings(Menu &menu, uint32_t rate=0, int nchannels=16,
		   float gain=0, float pregain=1,
		   Input::SOURCE source=Input::SINGLE_ENDED,
		   size_t nsource=Input::MaxSource,
		   bool reverse=true);

  // Return number of channels.
  // The number of channels is set from the configuration file
  // but needs to be passed manually to appropriate Control instances.
  uint8_t nchannels() const { return NChannels.value(); };
  
  // Set the number of channels.
  void setNChannels(uint8_t nchannels);

  // Return gain in dB.
  // The gain is set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  float gainDecibel() const { return Gain.value(); };
  
  // Set gain in dB.
  // The gain is also set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  void setGainDecibel(float gain);

  /* Whether order of input pins should be reversed. */
  bool reverseInputs() const { return ReverseInputs.enumValue(); };

  /* Set whether order of input pins should be reversed. */
  void setReverseInputs(bool reverse);

  using Menu::transmit;

  /* Transmit Rate and Gain via storage. */
  void transmit(Storage &storage);

    
protected:

  NumberParameter<uint8_t> NChannels;
  NumberParameter<float> Gain;
  BoolParameter ReverseInputs;
  
};

#endif
