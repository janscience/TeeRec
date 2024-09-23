// Use Teensy 3.5/3.6

// select a library for the TFT display:
//#define ST7735_T3
#define ST7789_T3
//#define ILI9341_T3  // XXX does not compile yet
//#define ILI9488_T3
//#define ST7735_ADAFRUIT
//#define ST7789_ADAFRUIT
//#define ILI9341_ADAFRUIT

#include <InputADC.h>
#include <AudioMonitor.h>
#include <Display.h>
  

// Default settings: --------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 4;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 44100;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1

uint updateScreen = 500;             // milliseconds
float displayTime = 0.01;
//float displayTime = 0.001*updateScreen;

// Pin assignment: ----------------------------------------------------

#define AMPL_ENABLE_PIN 32   // pin for enabling an audio amplifier
#define VOLUME_UP_PIN   25   // pin for push button for increasing audio volume
#define VOLUME_DOWN_PIN 26   // pin for push button for decreasing audio volume

// pins to control TFT display:
#define TFT_SCK   13
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_CS    10  
#define TFT_RST    8 // 9
#define TFT_DC     7 // 8 
#define TFT_BL    30 // backlight PWM, -1 to not use it
#include <AllDisplays.h>     // edit this file for your TFT monitor

// ---------------------------------------------------------------------

DATA_BUFFER(AIBuffer, NAIBuffer, 256*256);
InputADC aidata(AIBuffer, NAIBuffer);

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
  AudioMemory(16);
  audio.setupAmp(AMPL_ENABLE_PIN);
  audio.setupVolume(0.1, VOLUME_UP_PIN, VOLUME_DOWN_PIN);
  // uncomment at least the first line if you use the Teensy audio shield
  //audioshield.enable();
  //audioshield.volume(0.5);
  //audioshield.muteHeadphone();
  //audioshield.muteLineout();
  //audioshield.lineOutLevel(31);
}


void setupScreen() {
  int nplots = aidata.nchannels();
  if (nplots > 8)
    nplots = 8;
  screen.setPlotAreas(nplots, 0.0, 0.0, 1.0, 1.0);
  screenTime = 0;
  screen.setBacklightOn();
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


// --------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  aidata.check();
  initScreen(screen);
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
