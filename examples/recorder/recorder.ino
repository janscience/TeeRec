#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Display.h>
#include <RTClock.h>
#include <PushButtons.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
uint32_t samplingRate = 40000;      // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, -1, A4, A5, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, A17, -1, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

bool logging = true;                // keep saving to files
char fileName[] = "SDATELNUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;             // seconds

int startPin = 24;

int stimulusFrequency = 200;         // Hertz
int signalPins[] = {7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

SDWriter file;
size_t fileSamples = 0;


Display screen(1);
elapsedMillis screenTime;

RTClock rtclock;
PushButtons buttons;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
  float bt = aidata.bufferTime();
  if (bt < 1.0)
    Serial.printf("buffer time: %.0fms\n", 1000.0*bt);
  else
    Serial.printf("buffer time: %.2fs\n", bt);
}


void openNextFile() {
  String name = rtclock.makeStr(fileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0 )
    return;
  char datetime[20];
  rtclock.dateTime(datetime);
  file.openWave(name.c_str(), aidata, -1, datetime);
  fileSamples = 0;
  writeData();
  if (file.isOpen()) {
    screen.clearText(0);                // 35ms!
    writeData();
    screen.writeText(1, name.c_str());  // 25ms
    writeData();
    Serial.println(name);
  }
}


void setupStorage() {
  if (file.available())
    file.dataDir("recordings");
  file.setWriteInterval(aidata);
}


void startWrite(int id) {
  // on button press:
  if (file.available() && !file.isOpen()) {
    aidata.setMaxFileSamples(0);
    aidata.startWrite();
    openNextFile();
  }
}


void stopWrite(int id) {
  // on button release:
  aidata.setMaxFileTime(fileSaveTime);
}


void writeData() {
  fileSamples += aidata.writeData(file.file());
}


void splashScreen() {
  screen.setTextArea(0, 0.1, 0.75, 0.9, 0.95);
  screen.setTextArea(1, 0.0, 0.1, 0.4, 0.7, true);
  screen.setTextArea(2, 0.4, 0.1, 1.0, 0.7, true);
  screen.writeText(0, "TeeRec recorder");
  screen.writeText(1, "rate:\nres.:\nADC0:\nADC1:");
  char msg[100];
  sprintf(msg, "%.0fkHz\n%dbit\n%d channels\n%d channels",
          0.001*aidata.rate(), aidata.resolution(), aidata.nchannels(0), aidata.nchannels(1));
  screen.writeText(2, msg);
  delay(1000);
  screen.clearText();
}


void setupScreen() {
  screen.setTextArea(0, 0.0, 0.8, 1.0, 1.0);
  screen.setTextArea(1, 0.0, 0.8, 0.65, 1.0);
  screen.setTextArea(2, 0.71, 0.8, 1.0, 1.0);
  int nplots = aidata.nchannels();
  if (nplots > 8)
    nplots = 8;
  screen.setPlotAreas(nplots, 0.0, 0.0, 1.0, 0.8);
  screenTime = 0;
}


void setupButtons() {
  buttons.add(startPin, INPUT_PULLUP, startWrite, stopWrite);
}


void plotData() {   // 85ms
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    // text: 36ms
    if (file.isOpen())
      screen.scrollText(1);
    else {
      char ts[20];
      rtclock.dateTime(ts);
      ts[strlen(ts)-3] = '\0';
      screen.writeText(0, ts);
    }
    screen.clearPlots();   // 16ms
    writeData();
    size_t n = aidata.frames(displayTime);
    float data[n];
    size_t start = aidata.currentSample(n);
    for (int k=0; k<aidata.nchannels(); k++) {
      aidata.getData(k, start, data, n);
      screen.plot(k%screen.numPlots(), data, n, k/screen.numPlots()); // 8ms for n=500
    }
  }
}


void storeData() {
  if (file.needToWrite()) {
    writeData();
    if (aidata.endWrite()) {
      file.closeWave(fileSamples);
      if (logging)
        openNextFile();
      else {
        screen.clearText(1);
        screen.clearText(2);
      }
    }
    if (file.isOpen()) {
      char ts[6];
      aidata.fileTimeStr(ts);
      screen.writeText(2, ts);
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  rtclock.check();
  setupTestSignals(signalPins, stimulusFrequency);
  setupButtons();
  setupADC();
  splashScreen();
  setupScreen();
  setupStorage();
  screenTime = 0;
  aidata.start();
}


void loop() {
  buttons.update();
  storeData();
  plotData();
} 
