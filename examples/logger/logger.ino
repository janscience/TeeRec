#include <ContinuousADC.h>
#include <SDWriter.h>
#include <RTClock.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                        // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 40000;        // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, A4, A5, A6, A7, A8, A9, -1};      // input pins for ADC0
int8_t channels1 [] =  {A16, A17, A18, A19, A20, A22, A10, A11, -1};  // input pins for ADC1

char fileName[] = "data-ANUM.wav";    // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 60;              // seconds

int signalPins[] = {5, 4, 3, 2, -1};  // pins where to put out test signals
int stimulusFrequency = 500;          // Hertz


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;
SDWriter file;
WaveHeader wave;
RTClock rtclock;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
}


void openNextFile() {
  String name = rtclock.makeStr(fileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0)
    return;
  char dts[20];
  rtclock.dateTime(dts);
  file.openWave(name.c_str(), aidata, -1, dts);
  aidata.writeData(file.file());
  Serial.println(name);
}


void setupStorage() {
  file.dataDir("recordings");
  file.setWriteInterval(aidata);
  aidata.startWrite();
  openNextFile();
}


void storeData() {
  if (file.needToWrite()) {
    aidata.writeData(file.file());
    if (aidata.endWrite()) {
      file.close();  // file size was set by openWave()
      openNextFile();
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  rtclock.check();
  setupTestSignals(signalPins, stimulusFrequency);
  setupADC();
  setupStorage();
  aidata.start();
}


void loop() {
  storeData();
} 
