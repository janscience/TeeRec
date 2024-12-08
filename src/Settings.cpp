#include <Settings.h>


Settings::Settings(const char *path, int deviceid, const char *filename,
		   float filetime, float initialdelay, bool randomblinks,
		   float pulsefrequency, float displaytime,
		   float sensorsinterval) :
  Configurable("Settings"),
  Path(*this, "Path", path),
  ID(*this, "DeviceID", deviceid, 0, 128, "%d"),
  FileName(*this, "FileName", filename),
  FileTime(*this, "FileTime", filetime, 1.0, 8640.0, "%.0f", "s"),
  InitialDelay(*this, "InitialDelay", initialdelay, 0.0, 1e8, "%.0f", "s"),
  RandomBlinks(*this, "RandomBlinks", randomblinks),
  PulseFrequency(*this, "PulseFreq", pulsefrequency, 1e-2, 1e8, "%.0f", "Hz"),
  DisplayTime(*this, "DisplayTime", displaytime, 0.001, 10, "%.0f", "s", "ms"),
  SensorsInterval(*this, "SensorsInterval", sensorsinterval, 0.001, 1e8, "%.1f", "s") {
  ID.setSpecial(0, "device");
  InitialDelay.disable();
  RandomBlinks.disable();
  PulseFrequency.disable();
  DisplayTime.disable();
  SensorsInterval.disable();
}


void Settings::setPath(const char *path) {
  Path.setValue(path);
}


void Settings::setDeviceID(int id) {
  ID.setValue(id);
}


void Settings::setFileName(const char *fname) {
  FileName.setValue(fname);
}


void Settings::setFileTime(float time) {
  FileTime.setValue(time);
}


void Settings::setInitialDelay(float time) {
  InitialDelay.setValue(time);
}


void Settings::setRandomBlinks(bool random) {
  RandomBlinks.setValue(random);
}


void Settings::setPulseFrequency(float freq) {
  PulseFrequency.setValue(freq);
}


void Settings::setDisplayTime(float time) {
  DisplayTime.setValue(time);
}


void Settings::setSensorsInterval(float time) {
  SensorsInterval.setValue(time);
}


