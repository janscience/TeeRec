#include <Configurator.h>
#include <ContinuousADC.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Settings.h>
#include <Blink.h>
#include <TestSignals.h>


// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file logger.cfg)

uint32_t samplingRate = 40000;       // samples per second and channel in Hertz
int bits = 12;                       // resolution: 10bit 12bit, or 16bit
int averaging = 4;                   // number of averages per sample: 0, 4, 8, 16, 32
ADC_CONVERSION_SPEED convs = ADC_CONVERSION_SPEED::HIGH_SPEED;
ADC_SAMPLING_SPEED sampls = ADC_SAMPLING_SPEED::HIGH_SPEED;
int8_t channels0 [] =  {A1, -1, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A2, -1, A16, A17, A18, A19, A20, A22, A12, A13};  // input pins for ADC1

char fileName[] = "SDATELNUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;             // seconds

float initialDelay = 2.0;            // seconds

int pulseFrequency = 230;            // Hertz
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
Blink blink;


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
  String name = rtclock.makeStr(settings.FileName, true);
  if (name != prevname) {
    file.resetFileCounter();
    prevname = name;
  }
  name = file.incrementFileName(name);
  if (name.length() == 0) {
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted.");
    Serial.println();
    return false;
  }
  char dts[20];
  rtclock.dateTime(dts);
  file.openWave(name.c_str(), aidata, -1, dts);
  file.writeData();
  Serial.println(name);
  if (file.isOpen()) {
    if (fileSaveTime < 30)
      blink.set(2000, 100);
    else
      blink.set(5000, 100);
    blink.blink(2000, 1000);
    return true;
  }
  else {
    Serial.println();
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted.");
    return false;
  }
}


void setupStorage() {
  if (file.dataDir(settings.Path))
    Serial.printf("Save recorded data in folder \"%s\".\n\n", settings.Path);
  file.setWriteInterval(aidata);
  file.setMaxFileTime(settings.FileTime);
  file.startWrite();
  openNextFile();
}


void storeData() {
  if (file.needToWrite()) {
    size_t samples = file.writeData();
    if (samples == 0) {
      blink.clear();
      Serial.println();
      Serial.println("ERROR: data acquisition not running.");
      Serial.println("sampling rate probably too high,");
      Serial.println("given the number of channels, averaging, sampling and conversion speed.");
      while (1) {};
    }
    if (file.endWrite()) {
      file.close();  // file size was set by openWave()
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
    uint32_t delayblinks[] = {50, 150, 50, 2000, 0};
    blink.setDelayed(1000, delayblinks);
    blink.delay(uint32_t(1000.0*settings.InitialDelay));
  }
  else
    delay(uint32_t(1000.0*settings.InitialDelay));
  setupStorage();
}


void loop() {
  storeData();
  blink.update();
}
