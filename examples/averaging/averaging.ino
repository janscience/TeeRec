/* Find the optimal combination of averaging, conversion and sampling speed 
 * for a given sampling rate, resolution, and channel configuration.
 * 
 * First, connect the analog input channels to a fixed voltage
 * or short circuit them in your actual hardware configuration.
 * 
 * Connect the Teensy via USB to your computer and optionally insert an SD card.
 * 
 * Wait some time to the let the hardware settle in.
 * 
 * Modify resolution, sampling rate and channel configuration, as well as
 * conversion and sampling speeds to check for of this sketch according to
 * your requirements.
 * 
 * Open the serial monitor.
 * 
 * Then run this sketch until it tells you it finished (takes a while).
 * 
 * The sketch reports the standard deviation of the recorded raw integer
 * data for each setting and channel. The lower, the better. Ideally,
 * standard deviation should be smaller than one. Choose from the table
 * the best settings und use them for your application.
 * 
 * Beware, the estimates of the standard deviations vary. Run the test
 * a few times to get an idea of the variability.
 * 
 * If an SD card was inserted, the sketch records for each combination of
 * averaging, conversion and sampling speed one data buffer to files,
 * stored in the tests/ directory of the SD card. You may run the
 * utils/noise.py script on these files to display histograms and to
 * evaluate the noise levels in more detail.
 */ 

#include <TeensyADC.h>
#include <SDWriter.h>
#include <Blink.h>
#include <Watchdog.h>
// Install Watchdog library from Peter Polidoro via Library Manager.
// https://github.com/janelia-arduino/Watchdog

// Settings: --------------------------------------------------------

bool markdown = true;   // report as markdown table or plain text

int bits = 12;                   // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 44100;   // samples per second and channel in Hertz
int8_t channels0 [] =  {A5, -1, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};  // input pins for ADC0
int8_t channels1 [] =  {A10, -1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

const uint8_t maxConversionSpeeds = 3;
ADC_CONVERSION_SPEED conversionSpeeds[maxConversionSpeeds] = {
  ADC_CONVERSION_SPEED::VERY_HIGH_SPEED,
  ADC_CONVERSION_SPEED::HIGH_SPEED,
  ADC_CONVERSION_SPEED::MED_SPEED,
  // ADC_CONVERSION_SPEED::LOW_SPEED,
  // ADC_CONVERSION_SPEED::VERY_LOW_SPEED
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

// ------------------------------------------------------------------------------------------

uint8_t nchannels;
DMAMEM uint8_t convindex;
DMAMEM uint8_t samplindex;

DMAMEM uint8_t results_settings[maxConversionSpeeds*maxSamplingSpeeds*maxAverages][3];
DMAMEM double results_stdevs[maxConversionSpeeds*maxSamplingSpeeds*maxAverages][32];
DMAMEM size_t counter;
 
DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)
TeensyADC aidata(AIBuffer, NAIBuffer);

SDCard sdcard;
SDWriter file(sdcard, aidata);
Blink blink(LED_BUILTIN);
Watchdog watchdog;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.unsetScaling();
  aidata.setResolution(bits);
  aidata.setConversionSpeed(conversionSpeeds[convindex]);
  aidata.setSamplingSpeed(samplingSpeeds[samplindex]);
  aidata.check();
}


// standard deviation over first m samples of buffer of size n from channel c
double stdev(uint8_t c, size_t m, size_t n) {
  sample_t buffer[n];
  aidata.getData(c, 0, buffer, n);
  double a = 0.0;
  for (size_t k=0; k<m; k++)
    a += (double(buffer[k]) - a ) / double(k+1);
  double v = 0.0;
  double ep = 0.0;
  for (size_t k=0; k<m; k++) {
    double s = double(buffer[k]) - a;
    v += s*s;
    ep += s;
  }
  v = (v - ep*ep/m)/(m-1);
  return sqrt(v);
}


void report(bool markdown) {
  Serial.println();
  Serial.println("Standard deviations in raw integers for each channel:");
  Serial.println();
  uint8_t nch = nchannels;
  if (channels0[0] >= 0 && channels1[0] >= 0)
    nch /= 2;
  char seps[4] = " ";
  char starts[4] = "";
  char ends[4] = "\n";
  if (markdown) {
    strcpy(seps, " | ");
    strcpy(starts, "| ");
    strcpy(ends, " |\n");
  }
  Serial.printf("%sconvers %ssampling%savrg", starts, seps, seps);
  for (uint8_t c=0; c<nch; c++) {
    char cs[4];
    if (channels0[0] >= 0 && channels0[c] >= 0) {
      aidata.channelStr(channels0[c], cs);
      Serial.printf("%s%4s", seps, cs);
    }
    if (channels1[0] >= 0 && channels1[c] >= 0) {
      aidata.channelStr(channels1[c], cs);
      Serial.printf("%s%4s", seps, cs);
    }
  }
  Serial.print(ends);
  if (markdown) {
    Serial.printf("%s:-------%s:-------%s---:", starts, seps, seps);
    for (uint8_t c=0; c<nch; c++) {
      if (channels0[0] >= 0 && channels0[c] >= 0)
        Serial.printf("%s---:", seps);
    if (channels1[0] >= 0 && channels1[c] >= 0)
        Serial.printf("%s---:", seps);
    }
    Serial.print(ends);
  }
  for (size_t j=0; j<counter; j++) {
    Serial.printf("%s%-8s", starts, aidata.conversionSpeedShortStr(conversionSpeeds[results_settings[j][0]]));
    Serial.printf("%s%-8s", seps, aidata.samplingSpeedShortStr(samplingSpeeds[results_settings[j][1]]));
    Serial.printf("%s%4i", seps, results_settings[j][2]);
    for (uint8_t c=0; c<nchannels; c++)
      Serial.printf("%s%4.1f", seps, results_stdevs[j][c]);
    Serial.print(ends);
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  arm_dcache_flush(&convindex, sizeof(convindex));
  arm_dcache_flush(&samplindex, sizeof(samplindex));
  arm_dcache_flush(results_settings, sizeof(results_settings));
  arm_dcache_flush(results_stdevs, sizeof(results_stdevs));
  arm_dcache_flush(&counter, sizeof(counter));
  nchannels = 0;
  for (uint8_t k=0; k<16; k++) {
    if (channels0[k] < 0)
      break;
    else
      nchannels++;
  }
  for (uint8_t k=0; k<16; k++) {
    if (channels1[k] < 0)
      break;
    else
      nchannels++;
  }
  if (!watchdog.tripped()) {
    convindex = 0;
    samplindex = 0;
    counter = 0;
    memset(results_settings, 0, sizeof(results_settings));
    memset(results_stdevs, 0, sizeof(results_stdevs));
  }
  else {
    samplindex++;
    if (samplindex >= maxSamplingSpeeds) {
      samplindex = 0;
      convindex++;
      if (convindex >= maxConversionSpeeds) {
        Serial.println(">>> test finished <<<");
        report(markdown);
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
  delay(4000);
  watchdog.enable(Watchdog::TIMEOUT_4S);
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
    delay(2000*buffertime);
    aidata.stop();
    // We run it for 2 buffers,
    // but analyse only the first half of the second buffer.
    // This excludes startup and shutdown noise...
    file.start(aidata.nbuffer()/2);
    delay(50);
    float sampledtime = aidata.sampledTime();
    Serial.printf("  avrg=%2d: %5.3fsec", averages_list[k], sampledtime);
    if (sampledtime < 0.99*buffertime)
      while (1) {}; // wait for watchdog to restart
    size_t nframes = aidata.nbuffer()/aidata.nchannels();
    results_settings[counter][0] = convindex;
    results_settings[counter][1] = samplindex;
    results_settings[counter][2] = averages_list[k];
    for (uint8_t c=0; c<nchannels; c++)
      results_stdevs[counter][c] = stdev(c, nframes/2, nframes);
    counter++;
    file.openWave(fname, 0);
    if ( file.isOpen() ) {
      file.write();
      file.closeWave();
      Serial.printf(" -> saved to %s\n", fname);
      watchdog.reset();
      delay(1000);
    }
    else
      Serial.println();
  }
  while (1) {}; // wait for watchdog to restart
}
 
