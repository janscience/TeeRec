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

  static const size_t MaxStr = 127;
  char Path[MaxStr + 1];
  char FileName[MaxStr + 1];
  float FileTime;
  float PulseFrequency;
  float DisplayTime;
  float InitialDelay;
  float SensorsInterval;


protected:

  StringParameter<MaxStr+1> PathP;
  StringParameter<MaxStr+1> FileNameP;
  TimeParameter<float> FileTimeP;
  FrequencyParameter<float> PulseFrequencyP;
  TimeParameter<float> DisplayTimeP;
  TimeParameter<float> InitialDelayP;
  TimeParameter<float> SensorsIntervalP;
  
};

#endif
