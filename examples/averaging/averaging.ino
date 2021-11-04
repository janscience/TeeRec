/* Find the optimal combination of averaging, conversion and sampling speed 
 * for a given sampling rate, resolution, and channel configuration.
 * First, connect the analog input channels to a fixed voltage
 * or short circuit them in your final hardware configuration.
 * Insert an SD card.
 * Then run this sketch until it is finished. It records for each
 * combination of averaging, conversion and sampling speed 3.5sec
 * into files, stored in the tests/ directory of the SD card.
 * Then run the extras/noise.py script on these files to evaluate the
 * noise levels. Choose the setting with the lowest noise level
 * for your application.
 */ 

#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Blink.h>
#include <Watchdog.h>
// Install Watchdog library from Peter Polidoro via Library Manager.
// https://github.com/janelia-arduino/Watchdog

int bits = 12;                       // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 100000;       // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, -1, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, -1, A17, A18, A19, -1, A20, A22, A10, A11};  // input pins for ADC1

float fileSaveTime = 3.5;            // seconds, do not change!

const uint8_t maxConversionSpeeds = 5;
ADC_CONVERSION_SPEED conversionSpeeds[maxConversionSpeeds] = {
  ADC_CONVERSION_SPEED::VERY_HIGH_SPEED,
  ADC_CONVERSION_SPEED::HIGH_SPEED,
  ADC_CONVERSION_SPEED::MED_SPEED,
  ADC_CONVERSION_SPEED::LOW_SPEED,
  ADC_CONVERSION_SPEED::VERY_LOW_SPEED
};

const uint8_t maxSamplingSpeeds = 5;
ADC_SAMPLING_SPEED samplingSpeeds[maxSamplingSpeeds] = {
  ADC_SAMPLING_SPEED::VERY_HIGH_SPEED,
  ADC_SAMPLING_SPEED::HIGH_SPEED,
  ADC_SAMPLING_SPEED::MED_SPEED,
  ADC_SAMPLING_SPEED::LOW_SPEED,
  ADC_SAMPLING_SPEED::VERY_LOW_SPEED
};

DMAMEM uint8_t convindex;
DMAMEM uint8_t samplindex;

// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;
SDCard sdcard;
SDWriter file(sdcard, aidata);
Blink blink;
Watchdog watchdog;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setConversionSpeed(conversionSpeeds[convindex]);
  aidata.setSamplingSpeed(samplingSpeeds[samplindex]);
  aidata.check();
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  arm_dcache_flush(&convindex, sizeof(convindex));
  arm_dcache_flush(&samplindex, sizeof(samplindex));
  if (!watchdog.tripped()) {
    convindex = 0;
    samplindex = 0;
  }
  else {
    samplindex++;
    if (samplindex >= maxSamplingSpeeds) {
      samplindex = 0;
      convindex++;
      if (convindex >= maxConversionSpeeds) {
        Serial.println(">>> test finished <<<");
        while (1) {
          watchdog.reset();
          delay(1000);
        }
      }
    }
  }
  setupADC();
  sdcard.begin();
  file.dataDir("tests");
  file.setWriteInterval(aidata);
  file.setMaxFileTime(fileSaveTime);
  blink.set(1000, 20);
  delay(1000);
  watchdog.enable(Watchdog::TIMEOUT_4S);
}


void loop() {
  char fname[30];
  const char *convs = aidata.conversionSpeedShortStr();
  const char *sampls = aidata.samplingSpeedShortStr();
  Serial.printf("%s conversion speed, %s sampling speed:\n", convs, sampls);
  uint8_t averages_list[] = {1, 4, 8, 16, 32};
  for (unsigned int k=0; k<sizeof(averages_list); k++) {
    watchdog.reset();
    aidata.setAveraging(averages_list[k]);
    sprintf(fname, "averaging-%03.0fkHz-%02dbit-conv%s-sampl%s-avrg%02d.wav", 0.001*aidata.rate(),
            aidata.resolution(), convs, sampls, aidata.averaging());
    Serial.printf("  %s\n", fname);
    file.openWave(fname, aidata);
    float filetime = fileSaveTime;
    aidata.start();
    if ( file.isOpen() ) {
      elapsedMillis wtime;
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
    else {
      filetime = 1;
      delay(1000);
    }
    delay(50);
    float sampledtime = aidata.sampledTime();
    aidata.stop();
    delay(50);
    Serial.printf("  avrg=%d: %5.3fsec\n", averages_list[k], sampledtime);
    if (sampledtime < 0.99*filetime) {
      blink.switchOff();
      file.file().remove(fname);
      while (1) {};
    }
  }
}
 
