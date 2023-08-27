/*
  TeensyTDMSettings - configuration settings for TeensyTDM.
  Created by Jan Benda, Aug 19, 2023.
*/


#ifndef TeensyTDMSettings_h
#define TeensyTDMSettings_h


#include <Arduino.h>
#include <Configurable.h>


class TeensyTDMSettings : public Configurable {

public:

  // Constructor. Sets configuration name to "TDM".
  TeensyTDMSettings(uint32_t rate=0, float gain=0);

  // Constructor. Provides an instance of a TeensyTDM and
  // sets configuration name to "ADC".
  TeensyTDMSettings(TeensyTDM *tdm, uint32_t rate=0, float gain=0);

  // Constructor setting configuration name.
  TeensyTDMSettings(const char *name, uint32_t rate=0, float gain=0);

  // Constructor providing an instance of a TeensyTDM and
  // setting configuration name.
  TeensyTDMSettings(TeensyTDM *tdm, const char *name,
		    uint32_t rate=0, float gain=0);

  // Return sampling rate per channel in Hertz.
  // If a TeensyTDM instance is provided via the constructor,
  // the sampling rate is passed on to this instance by the configure()
  // function when parsing a configuration file.
  uint32_t rate() const { return Rate; };
  
  // Set sampling rate per channel in Hertz.
  // If a TeensyTDM instance is provided via the constructor,
  // the sampling rate is passed on to this instance by the configure()
  // function when parsing a configuration file.
  void setRate(uint32_t rate);

  // Return gain in dB.
  // The gain is set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  uint32_t gain() const { return Gain; };
  
  // Set gain in dB.
  // The gain is also set from the configuration file but needs to be passed
  // manually to appropriate Control instances.
  void setGain(float gain);
  
  // Configure TDM settings with the provided key-value pair.
  virtual void configure(const char *key, const char *val);

  // Apply TDM settings on tdm.
  // If no tdm is provided, the one provided to the constructor is used.
  void configure(TeensyTDM *tdm=0);

  // Transfer TDM settings from tdm to the TeensyTDMSettings instance.
  void setConfiguration(TeensyTDM *tdm=0);

  // Report current settings on Serial.
  void report() const;

    
protected:

  uint32_t Rate;
  float Gain;
  TeensyTDM *TDM;
  
};

#endif
