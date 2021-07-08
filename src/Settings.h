/*
  Settings - Some configurable general settings.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef Settings_h
#define Settings_h


#include <Configurable.h>


class Settings : public Configurable {

public:

  Settings(const char *path="recordings",
	   const char *filename="SDATELNUM.wav", float filetime=10.0,
	   float pulsefrequency=500.0, float displaytime=0.005);
  
  virtual void configure(const char *key, const char *val);

  static const size_t MaxStr = 100;
  char Path[MaxStr];
  char FileName[MaxStr];
  float FileTime;
  float PulseFrequency;
  float DisplayTime;
};

#endif
