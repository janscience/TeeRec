#include <Settings.h>


Settings::Settings(const char *path, const char *filename, float filetime,
		   float pulsefrequency, float displaytime,
		   float initialdelay) :
  Configurable("Settings") {
  strncpy(Path, path, MaxStr);
  strncpy(FileName, filename, MaxStr);
  FileTime = filetime;
  PulseFrequency = pulsefrequency;
  DisplayTime = displaytime;
  InitialDelay = initialdelay;
}


void Settings::configure(const char *key, const char *val) {
  char pval[MaxStr];
  if (strcmp(key, "path") == 0) {
    strncpy(Path, val, MaxStr);
    strcpy(pval, Path);
  }
  else if (strcmp(key, "filename") == 0) {
    char *sp = strrchr(val, '.');
    if (sp != NULL)
      *sp = '\0';    // truncate file extension
    strncpy(FileName, val, MaxStr);
    strcpy(pval, FileName);
  }
  else if (strcmp(key, "filetime") == 0) {
    FileTime = parseTime(val);
    sprintf(pval, "%.0fs", FileTime);
  }
  else if (strcmp(key, "displaytime") == 0) {
    DisplayTime = parseTime(val);
    sprintf(pval, "%.0fms", 1000.0*DisplayTime);
  }
  else if (strcmp(key, "pulsefreq") == 0) {
    PulseFrequency = parseFrequency(val);
    sprintf(pval, "%.0fHz", PulseFrequency);
  }
  else if (strcmp(key, "initialdelay") == 0) {
    InitialDelay = parseTime(val);
    sprintf(pval, "%.1fs", InitialDelay);
  }
  else {
    Serial.printf("  Settings key \"%s\" not found.\n", key);
    return;
  }
  Serial.printf("  set Settings-%s to %s\n", key, pval);
}
