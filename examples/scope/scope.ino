#include <ContinuousADC.h>
#include <Display.h>
#include "fonts/FreeSans7pt7b.h"
#include <Adafruit_ST7735.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
int averaging = 1;                   // number of averages per sample: , 4, 8, 16, 32
uint32_t samplingRate = 100000;      // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

int stimulusFrequency = 500;         // Hertz
int signalPins[] = {5, 4, 3, 2, -1}; // pins where to put out test signals


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

Display screen;
elapsedMillis screenTime;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setAveraging(averaging);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.check();
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


void setupScreen() {
  screen.setTextArea(0, 0.0, 0.8, 1.0, 1.0);
  char msg[30];
  sprintf(msg, "%d channels @ %.0fkHz, %dbits",
          aidata.nchannels(), 0.001*aidata.rate(), aidata.resolution());
  screen.writeText(0, msg);
  int nplots = aidata.nchannels();
  if (nplots > 8)
    nplots = 8;
  screen.setPlotAreas(nplots, 0.0, 0.0, 1.0, 0.8);
}


void plotData() {
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    screen.scrollText(0);
    screen.clearPlots();
    size_t n = aidata.frames(displayTime);
    float data[n];
    size_t start = aidata.currentSample(n);
    for (int k=0; k<aidata.nchannels(); k++) {
      aidata.getData(k, start, data, n);
      screen.plot(k%screen.numPlots(), data, n, k/screen.numPlots());
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  setupTestSignals(signalPins, stimulusFrequency);
  setupADC();
  initScreen();
  setupScreen();
  screenTime = 0;
  aidata.start();
}


void loop() {
  plotData();
} 
