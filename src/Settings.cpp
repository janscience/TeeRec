#include <Settings.h>


Settings::Settings(const char *path, const char *filename, float filetime,
		   float pulsefrequency, float displaytime,
		   float initialdelay, float sensorsinterval) :
  Configurable("Settings"),
  FileTime(filetime),
  PulseFrequency(pulsefrequency),
  DisplayTime(displaytime),
  InitialDelay(initialdelay),
  SensorsInterval(sensorsinterval),
  PathP(this, "Path", &Path),
  FileNameP(this, "FileName", &FileName),
  FileTimeP(this, "FileTime", &FileTime, "%.0f"),
  PulseFrequencyP(this, "PulseFreq", &PulseFrequency, "%.0f"),
  DisplayTimeP(this, "DisplayTime", &DisplayTime, "%.3f"),
  InitialDelayP(this, "InitialDelay", &InitialDelay, "%.0f"),
  SensorsIntervalP(this, "SensorsInterval", &SensorsInterval, "%.1f")
{
  strncpy(Path, path, MaxStr);
  strncpy(FileName, filename, MaxStr);
}
