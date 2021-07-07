#include <ContinuousADC.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <Blink.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
int averaging = 8;                   // number of averages per sample: 0, 4, 8, 16, 32
uint32_t samplingRate = 40000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, -1, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, -1, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

char fileName[] = "SDATELNUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;             // seconds

int stimulusFrequency = 200;         // Hertz
int signalPins[] = {7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;
SDWriter file(aidata);
WaveHeader wave;
RTClock rtclock;
Blink blink;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setAveraging(averaging);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.check();
}


void openNextFile() {
  String name = rtclock.makeStr(fileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0) {
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted.");
    Serial.println();
    return;
  }
  char dts[20];
  rtclock.dateTime(dts);
  file.openWave(name.c_str(), aidata, -1, dts);
  file.writeData();
  Serial.println(name);
  if (file.isOpen()) {
    blink.set(2000, 20);
    blink.blink(2000, 1000);
  }
  else {
    Serial.println();
    Serial.println("WARNING: failed to open file on SD card.");
    Serial.println("SD card probably not inserted.");
  }
}


void setupStorage() {
  file.dataDir("recordings");
  file.setWriteInterval(aidata);
  file.setMaxFileTime(fileSaveTime);
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
      while(1) {};
    }
    if (file.endWrite()) {
      file.close();  // file size was set by openWave()
      blink.clear();
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
  setupTestSignals(signalPins, stimulusFrequency);
  setupADC();
  blink.switchOff();
  setupStorage();
  aidata.start();
  aidata.report();
}


void loop() {
  storeData();
  blink.update();
}
