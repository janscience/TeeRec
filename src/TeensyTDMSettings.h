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

  // Constructor setting configuration name.
  TeensyTDMSettings(const char *name, uint32_t rate=0, float gain=0);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate; };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return gain in dB.
  uint32_t gain() const { return Gain; };
  
  // Set gain in dB.
  void setGain(float gain);
  
  // Configure TDM settings with the provided key-value pair.
  virtual void configure(const char *key, const char *val);

  // Report current settings on Serial.
  void report() const;

    
protected:

  uint32_t Rate;
  float Gain;

};

#endif
