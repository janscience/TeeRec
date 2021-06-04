#include <ContinuousADC.h>
#include <SDWriter.h>
#include <RTClock.h>

// Settings: --------------------------------------------------------------------------------

int bits = 12;                  // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 40000;  // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, A4, A5, A6, A7, A8, A9, -1};      // input pins for ADC0
int8_t channels1 [] =  {A16, A17, A18, A19, A20, A22, A10, A11, -1};  // input pins for ADC1

int stimulusFrequency = 500;  // Hertz

char fileName[] = "data-ANUM.wav";
float fileSaveTime = 60;


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

SDWriter file;
elapsedMillis saveTime;
uint updateFile = 0;

RTClock rtclock;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
  Serial.printf("buffer time: %.3fs\n", aidata.bufferTime());
}


void openNextFile() {
  String name = rtclock.makeStr(fileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0)
    return;
  file.setupWaveHeader(aidata);
  file.openWave(name.c_str());
  Serial.println(name);
}


void setupStorage() {
  file.dataDir("recordings");
  updateFile = uint(250*aidata.bufferTime()); // a quarter of the buffer
  aidata.startWrite();
  openNextFile();
}


void setupTestStimulus() {
  // Teensy 3.5 & 3.6, pins with same frequency:
  int pinfreqs[] = {0, 0, 1, 2, 2, 3, 3, 1, 1, 3, 3};
  int pinfreqcount[] = {0, 0, 0, 0};
  int pinfreqdc[] = {128, 32, 224, 64};
  for (int pin=2; pin<=6; pin++) {
    pinMode(pin, OUTPUT);
    int freqi = pinfreqs[pin];
    analogWriteFrequency(pin, stimulusFrequency*freqi);
    analogWrite(pin, pinfreqdc[pinfreqcount[freqi]]);
    pinfreqcount[freqi]++;
  }
}


void storeData() {
  if (saveTime > updateFile) {
    saveTime -= updateFile;
    aidata.writeData(file.file());
    if (aidata.endWrite()) {
      file.closeWave();
      openNextFile();
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  rtclock.check();
  setupTestStimulus();
  setupADC();
  setupStorage();
  saveTime = 0;
  aidata.start();
}


void loop() {
  storeData();
} 
