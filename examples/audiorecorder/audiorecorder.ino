#include <InputADC.h>
#include <AudioMonitor.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <DeviceID.h>
#include <PushButtons.h>
#include <Blink.h>
#include <Configurator.h>
#include <Settings.h>
#include <InputADCSettings.h>

// Default settings: ----------------------------------------------------------
// (may be overwritten by config file teerec.cfg)

#define SAMPLING_RATE 96000 // samples per second and channel in Hertz
#define BITS             12 // resolution: 10bit 12bit, or 16bit
#define AVERAGING         1 // number of averages per sample: 0, 4, 8, 16, 32
#define CONVERSION    ADC_CONVERSION_SPEED::VERY_HIGH_SPEED
#define SAMPLING      ADC_SAMPLING_SPEED::MED_SPEED
#define REFERENCE     ADC_REFERENCE::REF_3V3
int8_t channels0 [] =  {A14, A15, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

#define PATH          "recordings" // folder where to store the recordings
#define DEVICEID      1            // device identifier
#define FILENAME      "teerecID-SDATETIME.wav" // may include ID, IDA, DATE, SDATE, TIME, STIME,

// Pin assignment: ----------------------------------------------------

#define AMPL_ENABLE_PIN 32   // pin for enabling an audio amplifier
#define VOLUME_UP_PIN 25     // pin for push button for increasing audio volume
#define VOLUME_DOWN_PIN 26   // pin for push button for decreasing audio volume
#define START_PIN 24         // pin for starting a recording

// ----------------------------------------------------------------------------

DATA_BUFFER(AIBuffer, NAIBuffer, 256*256);
InputADC aidata(AIBuffer, NAIBuffer, channels0, channels1);

AudioOutputI2S speaker;
// AudioControlSGTL5000 audioshield;  // uncomment if you use the Teensy audio shield
AudioMonitor audio(aidata, speaker);

SDCard sdcard;
SDWriter file(sdcard, aidata);

Configurator config;
InputADCSettings aisettings(SAMPLING_RATE, BITS, AVERAGING,
			    CONVERSION, SAMPLING, REFERENCE);
Settings settings(PATH, DEVICEID, FILENAME);

RTClock rtclock;
DeviceID deviceid(DEVICEID);
String prevname; // previous file name
int restarts = 0;

PushButtons buttons;
Blink blink(LED_BUILTIN);


void setupAudio() {
  AudioMemory(16);
  audio.setupAmp(AMPL_ENABLE_PIN);
  audio.setupVolume(0.1, VOLUME_UP_PIN, VOLUME_DOWN_PIN);
  // uncomment at least the first line if you use the Teensy audio shield
  //audioshield.enable();
  //audioshield.volume(0.5);
  //audioshield.muteHeadphone();
  //audioshield.muteLineout();
  //audioshield.lineOutLevel(31);
}


String makeFileName() {
  String name = deviceid.makeStr(settings.fileName());
  time_t t = now();
  name = rtclock.makeStr(name, t, true);
  if (name != prevname) {
    file.sdcard()->resetFileCounter();
    prevname = name;
  }
  name = file.sdcard()->incrementFileName(name);
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
  char dts[20];
  rtclock.dateTime(dts);
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


void startWrite(int id) {
  // on button press:
  if (file.available()) {
    if (!file.isOpen()) {
      String name = makeFileName();
      if (name.length() == 0) {
        Serial.println("-> halt");
        aidata.stop();
        while (1) {};
      }
      file.setMaxFileSamples(0);
      file.start();
      openNextFile(name);
    }
    else {
      file.closeWave();
      blink.clear();
      Serial.println("  stopped recording\n");
    }
  }
}


void setupButtons() {
  buttons.add(START_PIN, INPUT_PULLUP, startWrite);
}


void setupStorage() {
  prevname = "";
  if (settings.fileTime() > 30)
    blink.setTiming(5000);
  if (file.sdcard()->dataDir(settings.path()))
    Serial.printf("Save recorded data in folder \"%s\".\n\n", settings.path());
  file.setWriteInterval();
  file.setMaxFileTime(settings.fileTime());
  file.header().setSoftware("TeeRec audiorecorder");
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
        char mfs[30];
        sprintf(mfs, "error%d-%d.msg", restarts+1, -samples);
        File mf = sdcard.openWrite(mfs);
        mf.close();
      }
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
  setupButtons();
  sdcard.begin();
  settings.disable("InitialDelay");
  settings.disable("DisplayTime");
  settings.disable("SensorsInterval");
  config.setConfigFile("teerec.cfg");
  config.load(sdcard);
  if (Serial)
    config.configure(Serial, 10000);
  config.report();
  deviceid.report();
  setupStorage();
  aisettings.configure(&aidata);
  aidata.check();
  setupAudio();
  aidata.start();
  aidata.report();
  blink.switchOff();
}


void loop() {
  buttons.update();
  storeData();
  audio.update();
  blink.update();
}
