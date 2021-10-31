#include <Configurator.h>
#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Display.h>
#include "fonts/FreeSans6pt7b.h"
#include "fonts/FreeSans7pt7b.h"
#include "fonts/FreeSans8pt7b.h"
#define ST7735
//#define ILI9341
#if defined(ST7735)
  #include <Adafruit_ST7735.h>       // 1.44""
#elif defined(ILI9341)
  #include "Adafruit_ILI9341.h"      // 2.8" with touch
  #include <Wire.h>
  #include <Adafruit_FT6206.h>
#endif
#include <RTClock.h>
#include <Settings.h>
#include <PushButtons.h>
#include <TestSignals.h>
  

// Default settings: -----------------------------------------------------------------------
// (may be overwritten by config file recorder.cfg)

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 4;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 100000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A4, -1, A4, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

bool logging = false;                // keep saving to files
char fileName[] = "SDATELNUM.wav";   // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;             // seconds

int startPin = 16;

int pulseFrequency = 500;            // Hertz
#ifdef TEENSY32
int signalPins[] = {3, 4, -1}; // pins where to put out test signals
#else
int signalPins[] = {7, 6, 5, 4, 3, 2, -1}; // pins where to put out test signals
#endif

// ------------------------------------------------------------------------------------------

Configurator config;
ContinuousADC aidata;
SDCard sdcard;
SDWriter file(sdcard, aidata);

Display screen;
#if defined(ILI9341)
Adafruit_FT6206 touch = Adafruit_FT6206();
#endif
bool freezePlots = false;
elapsedMillis screenTime;

Settings settings("recordings", fileName, fileSaveTime, pulseFrequency, displayTime);
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
  aidata.setReference(ADC_REFERENCE::REF_3V3);
  aidata.check();
}


void openNextFile() {
  String name = rtclock.makeStr(settings.FileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0 )
    return;
  char datetime[20];
  rtclock.dateTime(datetime);
  file.openWave(name.c_str(), aidata, -1, datetime);
  file.writeData();
  if (file.isOpen()) {
    screen.clearText(0);                // 35ms!
    file.writeData();
    screen.writeText(1, name.c_str());  // 25ms
    file.writeData();
    Serial.println(name);
  }
}


void setupStorage() {
  if (file.available())
    file.dataDir(settings.Path);
  file.setWriteInterval(aidata);
  file.setMaxFileTime(settings.FileTime);
}


void startWrite(int id) {
  // on button press:
  if (file.available() && !file.isOpen()) {
    file.setMaxFileSamples(0);
    file.startWrite();
    openNextFile();
  }
}


void stopWrite(int id) {
  // on button release:
  file.setMaxFileTime(settings.FileTime);
}


void initScreen() {
  #define TFT_SCK   14 // 13
  #define TFT_MISO  12
  #define TFT_MOSI  7 // 11
  #define TFT_CS    2 // 10  
  #define TFT_RST   1 // 9
  #define TFT_DC    0 // 8 
#if defined(ST7735)
  // Adafruit 1.44" TFT hardware specific initialization:
  //Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  screen.init(tft, 3);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ILI9341)
  Adafruit_ILI9341 *tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);
  tft->begin();
  screen.init(tft, 1);
  screen.setDefaultFont(FreeSans8pt7b);
  if (! touch.begin(128)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }
#endif
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
  String convspeed = aidata.conversionSpeedShortStr();
  String samplspeed = aidata.samplingSpeedShortStr();
  char chans0[50];
  char chans1[50];
  aidata.channels(0, chans0);
  aidata.channels(1, chans1);
  if (chans0[0] == '\0')
    strcpy(chans0, "-");
  if (chans1[0] == '\0')
    strcpy(chans1, "-");
  float bt = aidata.bufferTime();
  char bts[20];
  if (bt < 1.0)
    sprintf(bts, "%.0fms\n", 1000.0*bt);
  else
    sprintf(bts, "%.2fs\n", bt);
  sprintf(msg, "%.0fkHz\n%dbit\n%d,%s,%s\n%s\n%s\n%s",
          0.001*aidata.rate(), aidata.resolution(), aidata.averaging(),
          convspeed.c_str(), samplspeed.c_str(), chans0, chans1, bts);
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
#if defined(ILI9341)
  freezePlots = touch.touched();
#endif
  if (screenTime > updateScreen && ! freezePlots) {
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
    file.writeData();
    size_t n = aidata.frames(settings.DisplayTime);
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
    file.writeData();
    if (file.endWrite()) {
      file.closeWave();
      if (logging)
        openNextFile();
      else {
        screen.clearText(1);
        screen.clearText(2);
      }
    }
    if (file.isOpen()) {
      char ts[6];
      file.fileTimeStr(ts);
      screen.writeText(2, ts);
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  rtclock.check();
  setupButtons();
  setupADC();
  sdcard.begin();
  //config.setConfigFile("recorder.cfg");
  config.configure(sdcard);
  setupTestSignals(signalPins, settings.PulseFrequency);
  aidata.check();
  initScreen();
  splashScreen();
  setupScreen();
  setupStorage();
  screenTime = 0;
  aidata.start();
  aidata.report();
}


void loop() {
  buttons.update();
  storeData();
  plotData();
}
