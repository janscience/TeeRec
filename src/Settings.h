/*
  Settings - common configurable settings (file name, path, etc.).
  Created by Jan Benda, July 8th, 2021.
*/

#ifndef Settings_h
#define Settings_h


#include <MicroConfig.h>

class DeviceID;


class Settings : public Menu {

public:

  Settings(Menu &menu, const char *label="logger", int deviceid=0,
	   const char *path="recordings", const char *filename="SDATELNUM.wav",
	   float filetime=10.0, float initialdelay=0.0, bool randomblinks=false,
	   float pulsefrequency=500.0, float displaytime=0.005,
	   float sensorsinterval=10.0, float blinktimeout=0.0);

  Settings(Menu &menu, const char *path="recordings", const char *filename="SDATELNUM.wav",
	   float filetime=10.0, float initialdelay=0.0, bool randomblinks=false,
	   float pulsefrequency=500.0, float displaytime=0.005,
	   float sensorsinterval=10.0, float blinktimeout=0.0);

  static const size_t MaxStr = 64;

  /* Label to be used for naming the recordings. */
  const char *label() const { return Label.value(); };

  /* Set label for naming the recordings to label. */
  void setLabel(const char *label);

  /* Device identifier. */
  int deviceID() const { return ID.value(); };

  /* Set device identifier to id. */
  void setDeviceID(int id);

  /* Path on SD card where to store the data. */
  const char *path() const { return Path.value(); };

  /* Set path on SD card where to store the data to path. */
  void setPath(const char *path);

  /* File name to be used to save the recorded data. */
  const char *fileName() const { return FileName.value(); };

  /* Set name template to be used to save the recorded data to fname. */
  void setFileName(const char *fname);

  /* Replace LABEL and ID in path and filename by the respective strings. */
  void preparePaths(const DeviceID &deviceid);

  /* Time in seconds the files will record data. */
  float fileTime() const { return FileTime.value(); };

  /* Set time the files will record data to time seconds. */
  void setFileTime(float time);

  /* Time in seconds until recording of data is started. */
  float initialDelay() const { return InitialDelay.value(); };

  /* Set initial delay to time seconds. */
  void setInitialDelay(float time);

  /* Whether LED should blink randomly and be stored to file. */
  bool randomBlinks() const { return RandomBlinks.value(); };

  /* Set whether LED should blink randomly. */
  void setRandomBlinks(bool random);

  /* Time in seconds after which the status LEDs are switched off. */
  float blinkTimeout() const { return BlinkTimeout.value(); };

  /* Set time after which the status LEDs are switched off to time seconds. */
  void setBlinkTimeout(float time);

  /* Base frequency of generated test pulses. */
  float pulseFrequency() const { return PulseFrequency.value(); };

  /* Set frequency of test pulses to freq Hertz. */
  void setPulseFrequency(float freq);

  /* Time in seconds after which the display will be updated. */
  float displayTime() const { return DisplayTime.value(); };

  /* Set update time for display to time seconds. */
  void setDisplayTime(float time);
  
  /* Time in seconds between sensor readings. */
  float sensorsInterval() const { return SensorsInterval.value(); };

  /* Set time between sensor readings to time seconds. */
  void setSensorsInterval(float time);


protected:

  StringParameter<MaxStr> Label;
  NumberParameter<int> ID;
  StringParameter<MaxStr> Path;
  StringParameter<MaxStr> FileName;
  NumberParameter<float> FileTime;
  NumberParameter<float> InitialDelay;
  BoolParameter RandomBlinks;
  NumberParameter<float> BlinkTimeout;
  NumberParameter<float> PulseFrequency;
  NumberParameter<float> DisplayTime;
  NumberParameter<float> SensorsInterval;
  
};

#endif
