// select a library for the TFT display:
#define ST7735_T3
//#define ST7789_T3
//#define ILI9341_T3  // XXX does not compile yet
//#define ILI9488_T3
//#define ST7735_ADAFRUIT
//#define ILI9341_ADAFRUIT

// define pins to control TFT display:
#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10  
#define TFT_RST   8 // 9
#define TFT_DC    7 // 8 

#include <ContinuousADC.h>
#include <AudioMonitor.h>
#include <Display.h>
#include "fonts/FreeSans6pt7b.h"
#include "fonts/FreeSans7pt7b.h"
#include "fonts/FreeSans8pt7b.h"
#if defined(ST7735_T3)
  #include <ST7735_t3.h>
#elif defined(ST7789_T3)
  #include <ST7789_t3.h>
#elif defined(ILI9341_T3)
  #include <ILI9341_t3.h>
#elif defined(ILI9488_T3)
  #include <ILI9488_t3.h>
#elif defined(ST7735_ADAFRUIT)
  #include <Adafruit_ST7735.h>
#elif defined(ILI9341_ADAFRUIT)
  #include "Adafruit_ILI9341.h"
#endif
  

// Default settings: -----------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 4;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 44000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A14, A15, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.01;
//float displayTime = 0.001*updateScreen;

int ampl_enable_pin = 32;      // pin for enabling an audio amplifier
int volume_up_pin = 25;        // pin for push button for increasing audio volume
int volume_down_pin = 26;      // pin for push button for decreasing audio volume

// ------------------------------------------------------------------------------------------

ContinuousADC aidata;

Display screen;
elapsedMillis screenTime;

AudioOutputI2S speaker;
// AudioControlSGTL5000 audioshield;  // uncomment if you use the Teensy audio shield
AudioMonitor audio(aidata, speaker);


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


void setupAudio() {
  audio.setup(ampl_enable_pin, 0.1, volume_up_pin, volume_down_pin);
  // uncomment at least the first line if you use the Teensy audio shield
  //audioshield.enable();
  //audioshield.volume(0.5);
  //audioshield.muteHeadphone();
  //audioshield.muteLineout();
  //audioshield.lineOutLevel(31);
}


void initScreen() {
#if defined(ST7735_T3)
  ST7735_t3 *tft = new ST7735_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  DisplayWrapper<ST7735_t3> *tftscreen = new DisplayWrapper<ST7735_t3>(tft);
  screen.init(tftscreen, 1);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ST7789_T3)
  ST7789_t3 *tft = new ST7789_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  DisplayWrapper<ST7789_t3> *tftscreen = new DisplayWrapper<ST7789_t3>(tft);
  screen.init(tftscreen, 3);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ILI9341_T3)
  ILI9341_t3 *tft = new ILI9341_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->begin();
  DisplayWrapper<ILI9341_t3> *tftscreen = new DisplayWrapper<ILI9341_t3>(tft);
  screen.init(tftscreen, 3);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ILI9488_T3)
  ILI9488_t3 *tft = new ILI9488_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->begin();
  DisplayWrapper<ILI9488_t3> *tftscreen = new DisplayWrapper<ILI9488_t3>(tft);
  screen.init(tftscreen, 3);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ST7735_ADAFRUIT)
  // Adafruit 1.44" TFT hardware specific initialization:
  //Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
  tft->initR(INITR_144GREENTAB);
  screen.init(tft, 3);
  screen.setDefaultFont(FreeSans7pt7b);
#elif defined(ILI9341_ADAFRUIT)
  Adafruit_ILI9341 *tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);
  tft->begin();
  screen.init(tft, 1);
  screen.setDefaultFont(FreeSans8pt7b);
#endif
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


void plotData() {   // 85ms
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    screen.clearPlots();   // 16ms
    size_t n = aidata.frames(displayTime);
    float data[n];
    size_t start = aidata.currentSample(n);
    for (int k=0; k<aidata.nchannels(); k++) {
      aidata.getData(k, start, data, n);
      screen.plot(k%screen.numPlots(), data, n, k/screen.numPlots()); // 8ms for n=500
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  aidata.check();
  initScreen();
  setupScreen();
  setupAudio();
  screenTime = 0;
  aidata.start();
  aidata.report();
}


void loop() {
  plotData();
  audio.update();
}
