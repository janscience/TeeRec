#include <ContinuousADC.h>
#include <SDWriter.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 20000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, -1, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, -1, A17, A18, A19, -1, A20, A22, A10, A11};  // input pins for ADC1

float fileSaveTime = 10;             // seconds

// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;
SDWriter file;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  file.dataDir("recordings");
  file.setWriteInterval(aidata);
}


void loop() {
  uint8_t averages_list[] = {1, 4, 8, 16, 32};
  for (unsigned int k=0; k<sizeof(averages_list); k++) {
    aidata.setAveraging(averages_list[k]);
    const char *convs = aidata.conversionSpeedShortStr();
    char fname[30];
    sprintf(fname, "averaging-%.0fkHz-%dbit-%s-a%d.wav", 0.001*aidata.rate(),
            aidata.resolution(), convs, aidata.averaging());
    Serial.println(fname);
    file.openWave(fname, aidata);
    aidata.start();
    if ( file.isOpen() ) {
      while (1) {
        if (file.needToWrite()) {
          aidata.writeData(file.file());
          if (aidata.endWrite()) {
            file.close();  // file size was set by openWave()
            break;
          }
        }    
      }
    }
    else
      delay(1000);
    aidata.stop();
    delay(50);
    size_t c0 = aidata.counter(0);
    size_t c1 = aidata.counter(1);
    Serial.printf("avrg=%d: %d %d\n", averages_list[k], c0, c1);
    if (c0 == 0 && c1 == 0)
      while (true) {};
  }
}
 
