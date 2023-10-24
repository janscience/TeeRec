/*
  Settings - common configurable settings (file name, path, etc.).
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Settings_h
#define Settings_h


#include <Configurable.h>
#include <Parameter.h>


class Settings : public Configurable {

public:

  Settings(const char *path="recordings",
	   const char *filename="SDATELNUM.wav", float filetime=10.0,
	   float pulsefrequency=500.0, float displaytime=0.005,
	   float initialdelay=0.0, float sensorsinterval=10.0);

  static const size_t MaxStr = 128;

  /* Path on SD card where to store the data. */
  const char *path() const { return Path.value(); };

  /* File name to be used to save the recorded data. */
  const char *fileName() const { return FileName.value(); };
  
  float FileTime;
  float PulseFrequency;
  float DisplayTime;
  float InitialDelay;
  float SensorsInterval;


protected:

  StringParameter<MaxStr> Path;
  StringParameter<MaxStr> FileName;
  TimeParameter<float> FileTimeP;
  FrequencyParameter<float> PulseFrequencyP;
  TimeParameter<float> DisplayTimeP;
  TimeParameter<float> InitialDelayP;
  TimeParameter<float> SensorsIntervalP;
  
};

#endif
