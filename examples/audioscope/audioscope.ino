#include <ContinuousADC.h>
#include <AudioPlayBuffer.h>
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
  

// Default settings: -----------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 8;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 40000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A7, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.01;
//float displayTime = 0.001*updateScreen;

// ------------------------------------------------------------------------------------------

ContinuousADC aidata;

Display screen;
#if defined(ILI9341)
Adafruit_FT6206 touch = Adafruit_FT6206();
#endif
elapsedMillis screenTime;


AudioPlayBuffer playdata(aidata);
AudioPlayMemory sound0;
AudioMixer4 mix;
AudioOutputI2S speaker;
AudioConnection ac1(playdata, 0, mix, 0);
AudioConnection aco(mix, 0, speaker, 0);
AudioControlSGTL5000 audioshield;


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
  AudioMemory(32);
  int enable_pin = 32;
  if ( enable_pin >= 0 ) {
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, HIGH); // turn on the amplifier
    delay(10);                      // allow time to wake up
  }
  audioshield.enable();
  audioshield.volume(0.5);
  //audioshield.muteHeadphone();
  //audioshield.muteLineout();
  audioshield.lineOutLevel(31);
  mix.gain(0, 0.1);
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
}
