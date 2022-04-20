#include <Configurator.h>
#include <ContinuousADC.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Settings.h>
#include <Blink.h>
#include <TestSignals.h>


// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file logger.cfg)

uint32_t samplingRate = 44000; // samples per second and channel in Hertz
int bits = 12;                 // resolution: 10bit 12bit, or 16bit
int averaging = 4;             // number of averages per sample: 0, 4, 8, 16, 32
ADC_CONVERSION_SPEED convs = ADC_CONVERSION_SPEED::HIGH_SPEED;
ADC_SAMPLING_SPEED sampls = ADC_SAMPLING_SPEED::HIGH_SPEED;
int8_t channels0 [] =  {A1, -1, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A2, -1, A16, A17, A18, A19, A20, A22, A12, A13};  // input pins for ADC1

char fileName[] = "SDATELNUM"; // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;       // seconds

float initialDelay = 2.0;      // seconds

int pulseFrequency = 230;      // Hertz
int signalPins[] = {9, 8, 7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals


// ------------------------------------------------------------------------------------------

Configurator config;
ContinuousADC aidata;
SDCard sdcard;
SDWriter file(sdcard, aidata);
Settings settings("recordings", fileName, fileSaveTime, pulseFrequency,
                  0.0, initialDelay);
RTClock rtclock;
String prevname; // previous file name
Blink blink(LED_BUILTIN);

int restarts = 0;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  //aidata.unsetScaling();
  aidata.setResolution(bits);
  aidata.setAveraging(averaging);
  aidata.setConversionSpeed(convs);
  aidata.setSamplingSpeed(sampls);
  aidata.check();
}


bool openNextFile() {
  blink.clear();
  time_t t = now();
  String name = rtclock.makeStr(settings.FileName, t, true);
  if (name != prevname) {
    file.resetFileCounter();
    prevname = name;
  }
  name = file.incrementFileName(name);
  if (name.length() == 0) {
    Serial.println("WARNING: failed to increment file name.");
    Serial.println("SD card probably not inserted -> halt");
    Serial.println();
    aidata.stop();
    while (1) {};
    return false;
  }
  name += ".wav";
  char dts[20];
  rtclock.dateTime(dts, t);
  if (! file.openWave(name.c_str(), -1, dts)) {
    Serial.println();
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted or full -> halt");
    aidata.stop();
    while (1) {};
    return false;
  }
  file.write();
  Serial.println(name);
  blink.setSingle();
  blink.blinkSingle(0, 1000);
  return true;
}


void setupStorage() {
  if (settings.FileTime > 30)
    blink.setTiming(5000);
  if (file.dataDir(settings.Path))
    Serial.printf("Save recorded data in folder \"%s\".\n\n", settings.Path);
  file.setWriteInterval();
  file.setMaxFileTime(settings.FileTime);
  file.start();
  openNextFile();
}


void storeData() {
  if (file.pending()) {
    ssize_t samples = file.write();
    if (samples <= 0) {
      blink.clear();
      Serial.println();
      Serial.println("ERROR in writing data to file:");
      switch (samples) {
        case 0:
          Serial.println("  Nothing written into the file.");
          Serial.println("  SD card probably full -> halt");
          aidata.stop();
          while (1) {};
          break;
        case -1:
          Serial.println("  File not open.");
          break;
        case -2:
          Serial.println("  File already full.");
          break;
        case -3:
          Serial.println("  No data available, data acquisition probably not running.");
          Serial.println("  sampling rate probably too high,");
          Serial.println("  given the number of channels, averaging, sampling and conversion speed.");
          break;
      }
      if (samples == -3) {
        aidata.stop();
        file.closeWave();
        char mfs[20];
        sprintf(mfs, "error%d-%d.msg", restarts+1, -samples);
        FsFile mf = sdcard.openWrite(mfs);
        mf.close();
      }
    }
    if (file.endWrite() || samples < 0) {
      file.close();  // file size was set by openWave()
      if (samples < 0) {
        restarts++;
        if (restarts >= 5) {
          Serial.println("ERROR: Too many file errors -> halt.");
          aidata.stop();
          while (1) {};
        }
      }
      if (samples == -3) {
        aidata.start();
        file.start();
      }
      openNextFile();
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  prevname = "";
  setupADC();
  sdcard.begin();
  rtclock.setFromFile(sdcard);
  rtclock.report();
  config.setConfigFile("logger.cfg");
  config.configure(sdcard);
  setupTestSignals(signalPins, settings.PulseFrequency);
  aidata.check();
  aidata.start();
  aidata.report();
  blink.switchOff();
  if (settings.InitialDelay >= 2.0) {
    delay(1000);
    blink.setDouble();
    blink.delay(uint32_t(1000.0*settings.InitialDelay)-1000);
  }
  else
    delay(uint32_t(1000.0*settings.InitialDelay));
  setupStorage();
}


void loop() {
  storeData();
  blink.update();
}
