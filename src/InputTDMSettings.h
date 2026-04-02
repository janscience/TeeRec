/*
  InputTDMSettings - configuration settings for InputTDM.
  Created by Jan Benda, Aug 19, 2023.
*/


#ifndef InputTDMSettings_h
#define InputTDMSettings_h


#include <InputSettings.h>


class InputTDMSettings : public InputSettings {

public:

  enum SOURCE : uint8_t {
    DIFFERENTIAL,
    SINGLE_ENDED,
    DIGITAL
  };
  
  static const size_t MaxSource = 3;
  static const char *SourceStrings[MaxSource];
  static const SOURCE SourceEnums[MaxSource];

  // Constructor. Sets configuration name to "ADC".
  InputTDMSettings(Menu &menu, uint32_t rate=0, int nchannels=16,
		   float gain=0, float pregain=1, SOURCE source=SINGLE_ENDED);

  // Return number of channels.
  // The number of channels is set from the configuration file
  // but needs to be passed manually to appropriate Control instances.
  uint8_t nchannels() const { return NChannels.value(); };
  
  // Set the number of channels.
  void setNChannels(uint8_t nchannels);

  // Return channel source.
  SOURCE source() const { return Source.enumValue(); };
  
  // Set the channel source.
  void setSource(SOURCE source);

  // Return gain in dB.
  // The gain is set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  float gainDecibel() const { return Gain.value(); };
  
  // Set gain in dB.
  // The gain is also set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  void setGainDecibel(float gain);

    
protected:

  NumberParameter<uint8_t> NChannels;
  EnumParameter<SOURCE> Source;
  NumberParameter<float> Gain;
  
};

#endif
