#include <ContinuousADC.h>
#include <Display.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                   // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 100000;  // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

int stimulusFrequency = 500;   // Hertz
uint updateScreen = 500;       // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

Display screen(1);
int n_plots = 1;
elapsedMillis screenTime;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.check();
  Serial.printf("buffer time: %.3fs\n", aidata.bufferTime());
}


void setupScreen() {
  screen.setTextArea(0, 0.0, 0.8, 1.0, 1.0);
  char msg[30];
  aidata.settingsStr(msg);
  screen.pushText(0, msg);
  n_plots = aidata.nchannels();
  if (n_plots > 8)
    n_plots = 8;
  float h = 0.8/n_plots;
  for (int k=0; k<n_plots; k++)
    screen.setDataArea(n_plots-k-1, 0.0, k*h, 1.0, (k+0.9)*h);
}


void plotData() {
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    screen.scrollText(0);
    screen.clearData();
    size_t n = aidata.frames(displayTime);
    float data[n];
    size_t start = aidata.currentSample(n);
    for (int k=0; k<aidata.nchannels(); k++) {
      aidata.getData(k, start, data, n);
      screen.plotData(k%n_plots, data, n, k/n_plots);
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  setupTestSignals(2, 6, stimulusFrequency);
  setupADC();
  setupScreen();
  screenTime = 0;
  aidata.start();
}


void loop() {
  plotData();
} 
