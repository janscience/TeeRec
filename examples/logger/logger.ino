// select a data source:
//#define TEENSYADC     // data are recorded from Teensy internal ADCs
#define PCM186X     // data are recorded by TI PCM186x chip via TDM

#if defined(TEENSYADC)
  #include <TeensyADC.h>
#elif defined(PCM186X)
  #include <Wire.h>
  #include <ControlPCM186x.h>
  #include <TeensyTDM.h>
#endif
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>
#include <TestSignals.h>
#include <Configurator.h>
#include <Settings.h>
#include <TeensyADCSettings.h>


// Default settings: ----------------------------------------------------------
// (may be overwritten by config file logger.cfg)
#if defined(TEENSYADC)
  #define SAMPLING_RATE 44100 // samples per second and channel in Hertz
  #define BITS             12 // resolution: 10bit 12bit, or 16bit
  #define AVERAGING         8 // number of averages per sample: 0, 4, 8, 16, 32
  #define CONVERSION    ADC_CONVERSION_SPEED::HIGH_SPEED
  #define SAMPLING      ADC_SAMPLING_SPEED::HIGH_SPEED
  #define REFERENCE     ADC_REFERENCE::REF_3V3
  //int8_t channels0 [] =  {A2, -1, A3, A4, A5, A6, A7, A8, A9, A10};      // input pins for ADC0
  int8_t channels0 [] =  {A10, -1, A3, A4, A5, A6, A7, A8, A9, A10};      // input pins for ADC0
  //int8_t channels1 [] =  {A10, -1, A11, A16, A17, A12, A13};  // input pins for ADC1
  int8_t channels1 [] =  {-1, A10, A11, A16, A17, A12, A13};  // input pins for ADC1
#elif defined(PCM186X)
  #define SAMPLING_RATE 48000 // samples per second and channel in Hertz
  #define GAIN 20.0            // dB
#endif

#define PATH          "recordings" // folder where to store the recordings
#define FILENAME      "SDATELNUM"  // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
#define FILE_SAVE_TIME 10   // seconds

#define INITIAL_DELAY  6.0  // seconds

#define PULSE_FREQUENCY 230 // Hertz
int signalPins[] = {9, 8, 7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals


// ----------------------------------------------------------------------------

#ifdef TEENSY4
DATA_BUFFER(AIBuffer, NAIBuffer, 512*256)
#else
DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)
#endif
#if defined(TEENSYADC)
TeensyADC aidata(AIBuffer, NAIBuffer, channels0, channels1);
#elif defined(PCM186X)
ControlPCM186x pcm;
TeensyTDM aidata(AIBuffer, NAIBuffer);
#endif

SDCard sdcard;
SDWriter file(sdcard, aidata);

Configurator config;
#if defined(TEENSYADC)
TeensyADCSettings aisettings(SAMPLING_RATE, BITS, AVERAGING,
			     CONVERSION, SAMPLING, REFERENCE);
#endif
Settings settings(PATH, FILENAME, FILE_SAVE_TIME, PULSE_FREQUENCY,
                  0.0, INITIAL_DELAY);
RTClock rtclock;
String prevname; // previous file name
Blink blink(LED_BUILTIN);

int restarts = 0;


bool openNextFile() {
  blink.clear();
  time_t t = now();
  String name = rtclock.makeStr(settings.FileName, t, true);
  if (name != prevname) {
    file.sdcard()->resetFileCounter();
    prevname = name;
  }
  name = file.sdcard()->incrementFileName(name);
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
  if (file.sdcard()->dataDir(settings.Path))
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
        char mfs[30];
        sprintf(mfs, "error%d-%d.msg", restarts+1, -samples);
        File mf = sdcard.openWrite(mfs);
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


// -----------------------------------------------------------------------------

void setup() {
  blink.switchOn();
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  prevname = "";
  sdcard.begin();
  rtclock.setFromFile(sdcard);
  rtclock.report();
  config.setConfigFile("logger.cfg");
  config.configure(sdcard);
  setupTestSignals(signalPins, settings.PulseFrequency);
#if defined(TEENSYADC)
  aidata.configure(aisettings);
#elif defined(PCM186X)
  Wire.begin();
  pcm.begin();
  pcm.setMicBias(false, true);
  pcm.setupTDM(ControlPCM186x::CH1L, ControlPCM186x::CH1R, ControlPCM186x::CH2L, ControlPCM186x::CH2R, false);
  pcm.setGain(ControlPCM186x::ADCLR, GAIN);
  pcm.setFilters(ControlPCM186x::FIR, false);
  char ws[30];
  pcm.channelsStr(ws);
  file.header().setChannels(ws);
  pcm.gainStr(ControlPCM186x::ADC1L, ws);
  file.header().setGain(ws);
  aidata.setResolution(32);
  aidata.setRate(SAMPLING_RATE);
  aidata.setNChannels(4);   // TODO: take it from pcm!
  aidata.begin();
#endif
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
