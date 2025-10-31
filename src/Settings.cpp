#include <DeviceID.h>
#include <Settings.h>


Settings::Settings(Menu &menu, const char *label, int deviceid,
		   const char *path, const char *filename, float filetime,
		   float initialdelay, bool randomblinks,
		   float pulsefrequency, float displaytime,
		   float sensorsinterval, float blinktimeout) :
  Menu(menu, "Settings"),
  ID(*this, "DeviceID", deviceid, -1, 127, "%d"),
  Label(*this, "Label", label),
  Path(*this, "Path", path),
  FileName(*this, "FileName", filename),
  FileTime(*this, "FileTime", filetime, 1.0, 8640.0, "%.0f", "s"),
  InitialDelay(*this, "InitialDelay", initialdelay, 0.0, 1e8, "%.0f", "s"),
  RandomBlinks(*this, "RandomBlinks", randomblinks),
  BlinkTimeout(*this, "BlinkTimeout", blinktimeout, 0.0, 1e8, "%.0f", "s"),
  PulseFrequency(*this, "PulseFreq", pulsefrequency, 1e-2, 1e8, "%.0f", "Hz"),
  DisplayTime(*this, "DisplayTime", displaytime, 0.001, 10, "%.0f", "s", "ms"),
  SensorsInterval(*this, "SensorsInterval", sensorsinterval, 0.001, 1e8, "%.1f", "s") {
  ID.setSpecial(-1, "device");
  InitialDelay.disable();
  RandomBlinks.disable();
  BlinkTimeout.disable();
  PulseFrequency.disable();
  DisplayTime.disable();
  SensorsInterval.disable();
}


Settings::Settings(Menu &menu, const char *path, const char *filename,
		   float filetime, float initialdelay, bool randomblinks,
		   float pulsefrequency, float displaytime,
		   float sensorsinterval, float blinktimeout) :
  Settings(menu, "logger", 0, path, filename, filetime, initialdelay,
	   randomblinks, pulsefrequency, displaytime, sensorsinterval,
	   blinktimeout) {
  Label.disable();
  ID.disable();
}

								
void Settings::setDeviceID(int id) {
  ID.setValue(id);
}


void Settings::setLabel(const char *label) {
  Path.setValue(label);
}


void Settings::setPath(const char *path) {
  Path.setValue(path);
}


void Settings::setFileName(const char *fname) {
  FileName.setValue(fname);
}


void Settings::preparePaths(const DeviceID &deviceid) {
  // path:
  String s = Path.value();
  s.replace("LABEL", Label.value());
  s = deviceid.makeStr(s);
  Path.setValue(s.c_str());
  // filename:
  s = FileName.value();
  s.replace("LABEL", Label.value());
  s = deviceid.makeStr(s);
  FileName.setValue(s.c_str());
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


void Settings::setBlinkTimeout(float time) {
  BlinkTimeout.setValue(time);
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


