#include <ContinuousADC.h>
#include <Watchdog.h>
// Install Watchdog library from Peter Polidoro via Library Manager.
// https://github.com/janelia-arduino/Watchdog


int8_t channels0 [] =  {A2, A3, A4, A5, A6, A7, A8, A9, -1};      // input pins for ADC0
int8_t channels1 [] =  {A16, A17, A18, A19, A20, A22, A10, A11, -1};  // input pins for ADC1

DMAMEM uint32_t rate;
DMAMEM uint32_t ratestep;
DMAMEM uint8_t bits;
DMAMEM uint8_t nchannels;

DMAMEM uint16_t results[8][4];
DMAMEM uint16_t counter;
 
ContinuousADC aidata;
Watchdog watchdog;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(10000);
  aidata.setResolution(12);
  aidata.setAveraging(1);
  aidata.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  aidata.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  aidata.check();
}


void checkRate() {
  aidata.setRate(rate);
  aidata.setResolution(bits);
  channels0[nchannels] = -1;
  channels1[nchannels] = -1;
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.start();
  delay(1000);
  aidata.stop();
  delay(100);
  size_t c0 = aidata.counter(0);
  size_t c1 = aidata.counter(1);
  Serial.printf("%3.0fkHz @%2dbit, %2dchannels: %d %d\n",
                0.001*rate, bits, nchannels, c0, c1);
  ratestep /= 2;
  if (ratestep < 1000) {
    // store results:
    results[counter][0] = nchannels;
    results[counter][1] = nchannels;
    results[counter][2] = bits;
    results[counter][3] = uint16_t(0.001*rate);
    counter++;
    // next settings:
    ratestep = 500000;
    rate = 500000;
    nchannels /= 2;
    if (nchannels >= 1)
      return;
    nchannels = 8;
    if (bits == 16) {
      bits = 12;
      return;
    }
    else {
      // done, print results:
      Serial.println();
      Serial.printf("channels0 channels1 bits rate\n");
      for (uint16_t k=0; k<counter; k++)
        Serial.printf("%-9i %-9i %-4i %4i\n",
                      results[k][0], results[k][1], results[k][2], results[k][3]);
      while (true) {
        watchdog.reset();
        delay(200);
      }
    }
  }
  if (c0 == 0 || c1 == 0)
    rate -= ratestep;
  else
    rate += ratestep;
}


// ------------------------------------------------------------------------------------------

void setup() {
  arm_dcache_flush(&rate, sizeof(rate));
  arm_dcache_flush(&ratestep, sizeof(ratestep));
  arm_dcache_flush(&bits, sizeof(bits));
  arm_dcache_flush(&nchannels, sizeof(nchannels));
  arm_dcache_flush(&results, sizeof(results));
  arm_dcache_flush(&counter, sizeof(counter));
  if (!watchdog.tripped()) {
    ratestep = 500000;
    rate = 500000;
    bits = 16;
    nchannels = 8;
    counter = 0;
  }
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  watchdog.enable(Watchdog::TIMEOUT_2S);
  setupADC();
  checkRate();
}


void loop() {
}
