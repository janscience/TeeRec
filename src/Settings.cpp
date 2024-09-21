#include <Settings.h>


Settings::Settings(const char *path, const char *filename, float filetime,
		   float pulsefrequency, float displaytime,
		   float initialdelay, float sensorsinterval) :
  Configurable("Settings"),
  Path(*this, "Path", path),
  FileName(*this, "FileName", filename),
  FileTime(*this, "FileTime", filetime, "%.0f", "s"),
  PulseFrequency(*this, "PulseFreq", pulsefrequency, 1e-2, 1e8, "%.0f", "Hz"),
  DisplayTime(*this, "DisplayTime", displaytime, 0.001, 10, "%.0f", "s", "ms"),
  InitialDelay(*this, "InitialDelay", initialdelay, 0.0, 1e8, "%.0f", "s"),
  SensorsInterval(*this, "SensorsInterval", sensorsinterval, 0.001, 1e8, "%.1f", "s") {
}
