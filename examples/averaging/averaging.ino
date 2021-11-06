/* Find the optimal combination of averaging, conversion and sampling speed 
 * for a given sampling rate, resolution, and channel configuration.
 * 
 * First, connect the analog input channels to a fixed voltage
 * or short circuit them in your final hardware configuration.
 * 
 * Insert an SD card.
 * 
 * Then run this sketch until it tells you it finished.
 * The sketch records for each combination of averaging, conversion and
 * sampling speed one data buffer to files, stored in the tests/ directory
 * of the SD card.
 * 
 * Finally, run the extras/noise.py script on these files to evaluate the
 * noise levels. Choose the setting with the lowest noise level
 * for your application.
 */ 

#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Blink.h>
#include <Watchdog.h>
// Install Watchdog library from Peter Polidoro via Library Manager.
// https://github.com/janelia-arduino/Watchdog

int bits = 12;                   // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 40000;  // samples per second and channel in Hertz
const uint8_t nchannels = 2;     // always set this fitting to channels0 and channels1
int8_t channels0 [] =  {A2, -1, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {A16, -1, A17, A18, A19, -1, A20, A22, A10, A11};  // input pins for ADC1

const uint8_t maxConversionSpeeds = 5;
ADC_CONVERSION_SPEED conversionSpeeds[maxConversionSpeeds] = {
  ADC_CONVERSION_SPEED::VERY_HIGH_SPEED,
  ADC_CONVERSION_SPEED::HIGH_SPEED,
  ADC_CONVERSION_SPEED::MED_SPEED,
  ADC_CONVERSION_SPEED::LOW_SPEED,
  ADC_CONVERSION_SPEED::VERY_LOW_SPEED
};

/*
const uint8_t maxConversionSpeeds = 4;
ADC_CONVERSION_SPEED conversionSpeeds[maxConversionSpeeds] = {
  ADC_CONVERSION_SPEED::ADACK_2_4,
  ADC_CONVERSION_SPEED::ADACK_4_0,
  ADC_CONVERSION_SPEED::ADACK_5_2,
  ADC_CONVERSION_SPEED::ADACK_6_2
};
*/

const uint8_t maxSamplingSpeeds = 5;
ADC_SAMPLING_SPEED samplingSpeeds[maxSamplingSpeeds] = {
  ADC_SAMPLING_SPEED::VERY_HIGH_SPEED,
  ADC_SAMPLING_SPEED::HIGH_SPEED,
  ADC_SAMPLING_SPEED::MED_SPEED,
  ADC_SAMPLING_SPEED::LOW_SPEED,
  ADC_SAMPLING_SPEED::VERY_LOW_SPEED
};

const uint8_t maxAverages = 5;
uint8_t averages_list[maxAverages] = {1, 4, 8, 16, 32};


DMAMEM uint8_t convindex;
DMAMEM uint8_t samplindex;

DMAMEM uint8_t results_settings[maxConversionSpeeds*maxSamplingSpeeds*maxAverages][3];
DMAMEM double results_stdevs[maxConversionSpeeds*maxSamplingSpeeds*maxAverages][nchannels];
DMAMEM size_t counter;

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


// standard deviation over n samples from channel c
double stdev(uint8_t c, size_t n) {
  sample_t buffer[n];
  aidata.getData(c, 0, buffer, n);
  double m = 0.0;
  for (size_t k=0; k<n; k++)
    m += (double(buffer[k]) - m ) / double(k+1);
  double v = 0.0;
  double ep = 0.0;
  for (size_t k=0; k<n; k++) {
    double s = double(buffer[k]) - m;
    v += s*s;
    ep += s;
  }
  v = (v - ep*ep/n)/(n-1);
  return sqrt(v);
}


void report() {
  Serial.println();
  Serial.println("Standard deviations in 16bit integers for each channel:");
  Serial.println();
  Serial.printf("convers  sampling avrg");
  uint8_t nch = nchannels;
  if (channels0[0] >= 0 && channels1[0] >= 0)
    nch /= 2;
  for (uint8_t c=0; c<nch; c++) {
    char cs[4];
    if (channels0[0] >= 0 && channels0[c] >= 0) {
      aidata.channelStr(channels0[c], cs);
      Serial.printf(" %4s", cs);
    }
    if (channels1[0] >= 0 && channels1[c] >= 0) {
      aidata.channelStr(channels1[c], cs);
      Serial.printf(" %4s", cs);
    }
  }
  Serial.println();
  for (size_t j=0; j<counter; j++) {
    Serial.printf("%-8s", aidata.conversionSpeedShortStr(conversionSpeeds[results_settings[j][0]]));
    Serial.printf(" %-8s", aidata.samplingSpeedShortStr(samplingSpeeds[results_settings[j][1]]));
    Serial.printf(" %4i", results_settings[j][2]);
    for (uint8_t c=0; c<nchannels; c++)
      Serial.printf(" %4.0f", results_stdevs[j][c]);
    Serial.println();
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  arm_dcache_flush(&convindex, sizeof(convindex));
  arm_dcache_flush(&samplindex, sizeof(samplindex));
  arm_dcache_flush(&results_settings, sizeof(results_settings));
  arm_dcache_flush(&results_stdevs, sizeof(results_stdevs));
  arm_dcache_flush(&counter, sizeof(counter));
  if (!watchdog.tripped()) {
    convindex = 0;
    samplindex = 0;
    counter = 0;
  }
  else {
    samplindex++;
    if (samplindex >= maxSamplingSpeeds) {
      samplindex = 0;
      convindex++;
      if (convindex >= maxConversionSpeeds) {
        Serial.println(">>> test finished <<<");
        report();
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
  watchdog.enable(Watchdog::TIMEOUT_2S);
  delay(500);
}


void loop() {
  char fname[30];
  const char *convs = aidata.conversionSpeedShortStr();
  const char *sampls = aidata.samplingSpeedShortStr();
  Serial.printf("%s conversion speed, %s sampling speed:\n", convs, sampls);
  for (unsigned int k=0; k<sizeof(averages_list); k++) {
    watchdog.reset();
    blink.switchOn();
    delay(100);
    blink.switchOff();
    aidata.setAveraging(averages_list[k]);
    sprintf(fname, "averaging-%03.0fkHz-%02dbit-conv%s-sampl%s-avrg%02d.wav", 0.001*aidata.rate(),
            aidata.resolution(), convs, sampls, aidata.averaging());
    float buffertime = aidata.bufferTime();
    if (buffertime > 1.0)
      buffertime = 1.0;
    // record data without SD card writing (no artifacts):
    aidata.start();
    delay(1000*buffertime);
    aidata.stop();
    file.startWrite(aidata.nbuffer());
    delay(50);
    float sampledtime = aidata.sampledTime();
    Serial.printf("  avrg=%2d: %5.3fsec", averages_list[k], sampledtime);
    if (sampledtime < 0.99*buffertime)
      while (1) {}; // wait for watchdog to restart
    size_t nframes = file.available()/aidata.nchannels();
    results_settings[counter][0] = convindex;
    results_settings[counter][1] = samplindex;
    results_settings[counter][2] = averages_list[k];
    for (uint8_t c=0; c<nchannels; c++)
      results_stdevs[counter][c] = stdev(c, nframes);
    counter++;
    file.openWave(fname, aidata, 0);
    if ( file.isOpen() ) {
      file.writeData();
      file.closeWave();
      Serial.printf(" -> saved to %s\n", fname);
    }
    else
      Serial.println();
  }
  while (1) {}; // wait for watchdog to restart
}
 
