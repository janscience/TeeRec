/* Tries different sampling rates to find out the maximum possible
 * sampling rate for various resolutions and number of channels to
 * be sampled from. Set averaging, conversion, and sampling speeds
 * at the beginning of the checkRate() function. Then compile the
 * code, upload it, and switch on the serial monitor. The sketch
 * reports what it is currently testing and finishes with a summary
 * table of the maximum sampling rates in kHz for the different
 * conditions.
 */

#include <InputADC.h>
#include <Watchdog.h>
// Install Watchdog library from Peter Polidoro via Library Manager.
// https://github.com/janelia-arduino/Watchdog


int8_t channels0[] =  {A2, A3, A4, A5, A6, A7, A8, A9, -1};      // input pins for ADC0
int8_t channels1[] =  {A16, A17, A18, A19, A20, A22, A10, A11, -1};  // input pins for ADC1
uint8_t bits[] = {10, 12, 16};    // bit depths to try

uint16_t measuretime = 1000;

DMAMEM uint32_t rate;
DMAMEM uint32_t ratestep;
DMAMEM uint32_t goodrate;
DMAMEM uint8_t bitindex;
DMAMEM uint8_t nchannels0;
DMAMEM uint8_t nchannels1;

DMAMEM uint16_t results[8][5];  // nchans0, nchans1, 10bit, 12bit, 16bit
DMAMEM uint16_t counter;
 
DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)
InputADC aidata(AIBuffer, NAIBuffer);

Watchdog watchdog;


void checkRate() {
  aidata.setAveraging(1);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.setRate(rate);
  aidata.setResolution(bits[bitindex]);
  channels0[nchannels0] = -1;
  channels1[nchannels1] = -1;
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.start();
  delay(measuretime);
  aidata.stop();
  delay(100);
  float sampledtime = aidata.sampledTime();
  Serial.printf("%3.0fkHz @%2dbit, %1d %1d channels: %5.3fsec\n",
                0.001*rate, bits[bitindex], nchannels0, nchannels1, sampledtime);
  ratestep /= 2;
  if (ratestep < 1000) {
    // store results:
    results[counter][0] = nchannels0;
    results[counter][1] = nchannels1;
    results[counter][2+bitindex] = uint16_t(0.001*goodrate);
    counter++;
    // next settings:
    ratestep = 500000;
    rate = 500000;
    if (nchannels1 == 0) {
      nchannels0 *= 2;
      if (nchannels0 <= 8)
        return;
      nchannels0 = 1;
      nchannels1 = 1;
      return;
    }
    else {
      nchannels0 *= 2;
      nchannels1 *= 2;
      if (nchannels0 <= 8)
        return;
      nchannels0 = 1;
      nchannels1 = 0;
    }
    counter = 0;
    if (bitindex > 0) {
      bitindex--;
      return;
    }
    else {
      // done, print results:
      Serial.println();
      Serial.printf("channels0 channels1 10bit 12bit 16bit\n");
      for (uint16_t k=0; k<8; k++) {
        Serial.printf("%-9i %-9i", results[k][0], results[k][1]);
        for (uint16_t i=2; i<5; i++)
          Serial.printf(" %5i", results[k][i]);
        Serial.println();
      }
      while (true) {
        watchdog.reset();
        delay(200);
      }
    }
  }
  if (1000*sampledtime < 0.99*measuretime)
    rate -= ratestep;
  else {
    goodrate = rate;
    rate += ratestep;
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  arm_dcache_flush(&rate, sizeof(rate));
  arm_dcache_flush(&ratestep, sizeof(ratestep));
  arm_dcache_flush(&rate, sizeof(goodrate));
  arm_dcache_flush(&bitindex, sizeof(bitindex));
  arm_dcache_flush(&nchannels0, sizeof(nchannels0));
  arm_dcache_flush(&nchannels1, sizeof(nchannels1));
  arm_dcache_flush(&results, sizeof(results));
  arm_dcache_flush(&counter, sizeof(counter));
  if (!watchdog.tripped()) {
    ratestep = 500000;
    rate = 500000;
    goodrate = 0;
    bitindex = 2;
    nchannels0 = 1;
    nchannels1 = 0;
    counter = 0;
  }
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  watchdog.enable(Watchdog::TIMEOUT_2S);
  checkRate();
}


void loop() {
}
