#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Blink.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 100000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, -1, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, -1, A17, A18, A19, -1, A20, A22, A10, A11};  // input pins for ADC1

ADC_CONVERSION_SPEED conversionSpeed = ADC_CONVERSION_SPEED::HIGH_SPEED;
// Choose one of:                                        
//   VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED, VERY_HIGH_SPEED,
//   ADACK_2_4, ADACK_4_0, ADACK_5_2, ADACK_6_2

ADC_SAMPLING_SPEED samplingSpeed = ADC_SAMPLING_SPEED::LOW_SPEED;
// Choose one of:                                        
//   VERY_LOW_SPEED, LOW_SPEED, LOW_MED_SPEED, MED_SPEED, MED_HIGH_SPEED,
//   HIGH_SPEED, HIGH_VERY_HIGH_SPEED, VERY_HIGH_SPEED

float fileSaveTime = 10;             // seconds

// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;
SDCard sdcard;
SDWriter file(sdcard, aidata);
Blink blink;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setConversionSpeed(conversionSpeed);
  aidata.setSamplingSpeed(samplingSpeed);
  aidata.check();
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  file.dataDir("tests");
  file.setWriteInterval(aidata);
  file.setMaxFileTime(fileSaveTime);
  blink.set(1000, 20);
  delay(4000);
}


void loop() {
  char fname[30];
  uint8_t averages_list[] = {1, 4, 8, 16, 32};
  for (unsigned int k=0; k<sizeof(averages_list); k++) {
    aidata.setAveraging(averages_list[k]);
    const char *convs = aidata.conversionSpeedShortStr();
    const char *sampls = aidata.samplingSpeedShortStr();
    sprintf(fname, "averaging-%03.0fkHz-%02dbit-conv%s-sampl%s-avrg%02d.wav", 0.001*aidata.rate(),
            aidata.resolution(), convs, sampls, aidata.averaging());
    Serial.println(fname);
    file.openWave(fname, aidata);
    aidata.start();
    if ( file.isOpen() ) {
      while (1) {
        blink.update();
        if (file.needToWrite()) {
          file.writeData();
          if (file.endWrite()) {
            file.close();  // file size was set by openWave()
            blink.blink(1000, 500);
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
    if (c0 == 0 && c1 == 0) {
      blink.switchOff();
      file.file().remove(fname);
      Serial.println(">>> test finished <<<");
      while (true) {};
    }
  }
}
 
