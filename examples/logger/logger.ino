#include <ContinuousADC.h>
#include <SDWriter.h>

// Settings: --------------------------------------------------------------------------------

uint32_t sampling_rate = 100000;  // samples per second and channel in Hertz

int8_t channels0 [] =  {A2, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

int stimulus_frequency = 500;  // Hertz

char file_name[] = "data-ANUM.wav";
float fileSaveTime = 60;


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

SDWriter file;
elapsedMillis saveTime;
uint updateFile = 0;
bool saving = false;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(sampling_rate);
  aidata.setResolution(12);  // 10bit 12bit, or 16bit 
  aidata.initBuffer(1024*64);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
  Serial.print("buffer time: ");
  Serial.print(aidata.bufferTime());
  Serial.println("s");
}


void openNextFile() {
  saving = false;
  if (!file.available())
    return;
  String name = file.incrementFileName(file_name);
  if (name.length() == 0 )
    return;
  file.setupWaveHeader(aidata);
  file.openWave(name.c_str());
  Serial.println(name);
  saving = true;
}


void setupStorage() {
  if (file.available())
    file.dataDir("recordings");
  updateFile = uint(250*aidata.bufferTime());
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
    analogWriteFrequency(pin, stimulus_frequency*freqi);
    analogWrite(pin, pinfreqdc[pinfreqcount[freqi]]);
    pinfreqcount[freqi]++;
  }
}


void writeData() {
  if (!file.available())
    return;
  aidata.writeData(file.file());
  char ts[6];
  aidata.fileTimeStr(ts);
}


void storeData() {
  if (saving && saveTime > updateFile) {
    saveTime -= updateFile;
    writeData();
    if (aidata.endWrite()) {
      file.closeWave();
      saving = false;
      openNextFile();
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);
  delay(100);
  setupTestStimulus();
  setupADC();
  setupStorage();
  saveTime = 0;
  aidata.start();
}


void loop() {
  storeData();
} 
