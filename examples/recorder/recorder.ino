#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Display.h>
#include "fonts/FreeSans6pt7b.h"
#include "fonts/FreeSans7pt7b.h"
#include "fonts/FreeSans8pt7b.h"
#include <Adafruit_ST7735.h>
#include <RTClock.h>
#include <PushButtons.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 4;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 20000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, -1, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {A16, A17, -1, A18, A19, A20, A22, A10, A11};  // input pins for ADC1, terminate with -1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

bool logging = false;                // keep saving to files
//char fileName[] = "SDATELNUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
char fileName[] = "AVG1-NUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;             // seconds

int startPin = 24;

int stimulusFrequency = 200;         // Hertz
int signalPins[] = {7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

SDWriter file;
size_t fileSamples = 0;

Display screen;
elapsedMillis screenTime;

RTClock rtclock;
PushButtons buttons;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setAveraging(averaging);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
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


void initScreen() {
  #define TFT_SCK   13
  #define TFT_MISO  12
  #define TFT_MOSI  11
  #define TFT_CS    10  
  #define TFT_RST   9
  #define TFT_DC    8 
  Adafruit_ST7735 *stscreen = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  stscreen->initR(INITR_144GREENTAB);
  screen.init(stscreen, 1);
  screen.setDefaultFont(FreeSans7pt7b);
}


void splashScreen() {
  screen.setTextArea(0, 0.0, 0.75, 1.0, 0.95);
  screen.setFont(0, FreeSans8pt7b);
  screen.setTextArea(1, 0.0, 0.0, 0.4, 0.7, true);
  screen.setFont(1, FreeSans6pt7b);
  screen.setTextArea(2, 0.4, 0.0, 1.0, 0.7, true);
  screen.setFont(2, FreeSans6pt7b);
  screen.writeText(0, "TeeRec recorder");
  screen.writeText(1, "rate:\nres.:\nspeed:\nADC0:\nADC1\nbuffer:");
  char msg[100];
  String convspeed = aidata.conversionSpeed();
  convspeed.replace("_SPEED", "");
  String samplspeed = aidata.samplingSpeed();
  samplspeed.replace("_SPEED", "");
  float bt = aidata.bufferTime();
  if (bt < 1.0)
    sprintf(msg, "%.0fkHz\n%dbit\n%d,%s,%s\n%d channels\n%d channels\n%.0fms",
            0.001*aidata.rate(), aidata.resolution(), aidata.averaging(), convspeed.c_str(), samplspeed.c_str(),
            aidata.nchannels(0), aidata.nchannels(1), 1000.0*bt);
  else
    sprintf(msg, "%.0fkHz\n%dbit\n%d,%s,%s\n%d channels\n%d channels\n%.2fms",
            0.001*aidata.rate(), aidata.resolution(), aidata.averaging(), convspeed.c_str(), samplspeed.c_str(),
            aidata.nchannels(0), aidata.nchannels(1), bt);
  screen.writeText(2, msg);
  delay(1500);
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
  initScreen();
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
