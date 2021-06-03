#include <ContinuousADC.h>
#include <Display.h>

// Settings: --------------------------------------------------------------------------------

uint32_t sampling_rate = 100000;  // samples per second and channel in Hertz

int8_t channels0 [] =  {A2, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

int stimulus_frequency = 500;  // Hertz
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
  aidata.setRate(sampling_rate);
  aidata.setResolution(12);  // 10bit 12bit, or 16bit 
  aidata.initBuffer(1024*64);
  aidata.setMaxFileTime(10);
  aidata.check();
  Serial.print("buffer time: ");
  Serial.print(aidata.bufferTime());
  Serial.println("s");
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


void plotData() {
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    screen.scrollText(0);
    screen.clearData();
    size_t n = aidata.samples(displayTime);
    float data[n];
    size_t start = aidata.currentIndex(n);
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
  setupTestStimulus();
  setupADC();
  setupScreen();
  screenTime = 0;
  aidata.start();
}


void loop() {
  plotData();
} 
