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

  Settings(const char *path="recordings", int deviceid=0,
	   const char *filename="SDATELNUM.wav", float filetime=10.0,
	   float pulsefrequency=500.0, float displaytime=0.005,
	   float initialdelay=0.0, float sensorsinterval=10.0);

  static const size_t MaxStr = 128;

  /* Path on SD card where to store the data. */
  const char *path() const { return Path.value(); };

  /* Device identifier. */
  int deviceID() const { return ID.value(); };

  /* File name to be used to save the recorded data. */
  const char *fileName() const { return FileName.value(); };

  /* Time in seconds the files will record data. */
  float fileTime() const { return FileTime.value(); };

  /* Base frequency of generated test pulses. */
  float pulseFrequency() const { return PulseFrequency.value(); };

  /* Time in seconds after which the display will be updated. */
  float displayTime() const { return DisplayTime.value(); };

  /* Time in seconds until recording of data is started. */
  float initialDelay() const { return InitialDelay.value(); };

  /* Time in seconds between sensor readings. */
  float sensorsInterval() const { return SensorsInterval.value(); };


protected:

  StringParameter<MaxStr> Path;
  NumberParameter<int> ID;
  StringParameter<MaxStr> FileName;
  NumberParameter<float> FileTime;
  NumberParameter<float> PulseFrequency;
  NumberParameter<float> DisplayTime;
  NumberParameter<float> InitialDelay;
  NumberParameter<float> SensorsInterval;
  
};

#endif
