#include <Configurator.h>
#include <Settings.h>
#include <ContinuousADC.h>
#include <AudioMonitor.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>
  

// Default settings: ----------------------------------------------------------
// (may be overwritten by config file teerec.cfg)

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 1;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 96000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A14, A15, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

char fileName[] = "teerec-SDATETIME";  // may include DATE, SDATE, TIME, STIME,
float fileSaveTime = 30;        // seconds

// ----------------------------------------------------------------------------

Configurator config;
Settings settings("recordings", fileName, fileSaveTime);

ContinuousADC aidata;

AudioPlayBuffer playdata(aidata);
AudioMonitor audio(&playdata);
AudioControlSGTL5000 audioshield;

SDCard sdcard;
SDWriter file(sdcard, aidata);

RTClock rtclock;
String prevname; // previous file name
int restarts = 0;

Blink blink(LED_BUILTIN);


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setAveraging(averaging);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.setReference(ADC_REFERENCE::REF_3V3);
  aidata.check();
}


void setupAudio() {
  playdata.setVolume(4);
  audio.setup(false, 32); // mono, amplifier enable on pin 32
  audioshield.enable();
  audioshield.volume(0.5);
  //audioshield.muteHeadphone();
  //audioshield.muteLineout();
  audioshield.lineOutLevel(31);
}


String makeFileName() {
  time_t t = now();
  String name = rtclock.makeStr(settings.FileName, t, true);
  if (name != prevname) {
    file.resetFileCounter();
    prevname = name;
  }
  name = file.incrementFileName(name);
  if (name.length() == 0) {
    Serial.println("WARNING: failed to increment file name.");
    Serial.println("SD card probably not inserted.");
    Serial.println();
    return "";
  }
  return name;
}


bool openNextFile(const String &name) {
  blink.clear();
  if (name.length() == 0)
    return false;
  String fname = name + ".wav";
  char dts[20];
  rtclock.dateTime(dts);
  if (! file.openWave(fname.c_str(), -1, dts)) {
    Serial.println();
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted or full -> halt");
    aidata.stop();
    while (1) {};
    return false;
  }
  file.write();
  Serial.println(fname);
  blink.setSingle();
  blink.blinkSingle(0, 1000);
  return true;
}


void setupStorage() {
  prevname = "";
  if (settings.FileTime > 30)
    blink.setTiming(5000);
  if (file.dataDir(settings.Path))
    Serial.printf("Save recorded data in folder \"%s\".\n\n", settings.Path);
  file.setWriteInterval();
  file.setMaxFileTime(settings.FileTime);
  file.setSoftware("TeeRec audiorecorder");
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
      String name = makeFileName();
      if (samples < 0) {
        restarts++;
        if (restarts >= 5) {
          Serial.println("ERROR: Too many file errors -> halt.");
          aidata.stop();
          while (1) {};
        }
      }
      if (samples == -3) {
        String sname = name + "-sensors";
        aidata.start();
        file.start();
      }
      openNextFile(name);
    }
  }
}


// ---------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  rtclock.report();
  setupADC();
  sdcard.begin();
  config.setConfigFile("teerec.cfg");
  config.configure(sdcard);
  setupStorage();
  aidata.check();
  setupAudio();
  aidata.start();
  aidata.report();
  blink.switchOff();
  String name = makeFileName();
  if (name.length() == 0) {
    Serial.println("-> halt");
    aidata.stop();
    while (1) {};
  }
  blink.setSingle();
  file.start();
  openNextFile(name);
}


void loop() {
  storeData();
  blink.update();
}
