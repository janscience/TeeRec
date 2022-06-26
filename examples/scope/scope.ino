// select a library for the TFT display:
//#define ST7735_T3
#define ST7789_T3
//#define ILI9341_T3  // XXX does not compile yet
//#define ILI9488_T3
//#define ST7735_ADAFRUIT
//#define ST7789_ADAFRUIT
//#define ILI9341_ADAFRUIT

// define pins to control TFT display:
#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10  
#define TFT_RST    8 // 9
#define TFT_DC     7 // 8 
#define TFT_BL    30 // backlight PWM, -1 to not use it

#include <ContinuousADC.h>
#include <Display.h>
#include <AllDisplays.h>       // edit this file for your TFT monitor
#include <TestSignals.h>


// Settings: ------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
int averaging = 1;                   // number of averages per sample: , 4, 8, 16, 32
uint32_t samplingRate = 100000;      // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, -1, A4, A5, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

int stimulusFrequency = 500;         // Hertz
int signalPins[] = {5, 4, 3, 2, -1}; // pins where to put out test signals


// ----------------------------------------------------------------------------

DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)

ContinuousADC aidata(AIBuffer, NAIBuffer);

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


void setupScreen() {
  screen.setTextArea(0, 0.0, 0.8, 1.0, 1.0);
  char msg[30];
  if (aidata.nchannels() == 1)
    sprintf(msg, "%d channel @ %.0fkHz, %dbits",
            aidata.nchannels(), 0.001*aidata.rate(), aidata.resolution());
  else
    sprintf(msg, "%d channels @ %.0fkHz, %dbits",
            aidata.nchannels(), 0.001*aidata.rate(), aidata.resolution());
  screen.writeText(0, msg);
  int nplots = aidata.nchannels();
  if (nplots > 8)
    nplots = 8;
  screen.setPlotAreas(nplots, 0.0, 0.0, 1.0, 0.8);
  screen.setBacklightOn();
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


// ----------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupTestSignals(signalPins, stimulusFrequency);
  setupADC();
  initScreen(screen);
  setupScreen();
  screenTime = 0;
  aidata.start();
  aidata.report();
}


void loop() {
  plotData();
}
