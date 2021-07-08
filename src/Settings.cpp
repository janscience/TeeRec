#include <Settings.h>


Settings::Settings(const char *path, const char *filename, float filetime,
		   float pulsefrequency, float displaytime) :
  Configurable("Settings") {
  strncpy(Path, path, MaxStr);
  strncpy(FileName, filename, MaxStr);
  FileTime = filetime;
  PulseFrequency = pulsefrequency;
  DisplayTime = displaytime;
}


void Settings::configure(const char *key, const char *val) {
  bool found = true;
  if (strcmp(key, "path") == 0)
    strncpy(Path, val, MaxStr);
  else if (strcmp(key, "filename") == 0)
    strncpy(FileName, val, MaxStr);
  else if (strcmp(key, "filetime") == 0)
    FileTime = parseTime(val);
  else if (strcmp(key, "displaytime") == 0)
    DisplayTime = parseTime(val);
  else if (strcmp(key, "pulsefreq") == 0)
    PulseFrequency = parseFrequency(val);
  else
    found = false;
  if (found)
    Serial.printf("  set Settings-%s to %s\n", key, val);
}
