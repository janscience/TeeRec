#include <ContinuousADC.h>
#include <AudioShield.h>
#include <Blink.h>
  

// Default settings: -----------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit 
int averaging = 1;                   // number of averages per sample: 0, 4, 8, 16, 32 - the higher the better, but the slowe
uint32_t samplingRate = 96000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A14, -1, A2, A3, A4, A5, A6, A7, A8, A9};      // input pins for ADC0, terminate with -1
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A13, A12, A11};  // input pins for ADC1, terminate with -1


// ------------------------------------------------------------------------------------------

ContinuousADC aidata;

AudioPlayBuffer playdata(aidata);
AudioShield audio(&playdata);

Blink blink(LED_BUILTIN);


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

// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  aidata.report();
  audio.setup();
  aidata.start();
  blink.setSingle();
  delay(2000);
  playdata.setMute(true);
  delay(2000);
  playdata.setMute(false);
}


void loop() {
  blink.update();
}
