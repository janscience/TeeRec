/* Report mean and standard deviation of recorded signal.
 * 
 * First, connect the analog input channels to a fixed voltage
 * or short circuit them in your actual hardware configuration.
 * 
 * Modify resolution, sampling rate, channel configuration, conversion and sampling speeds
 * of this sketch according to your requirements.
 * 
 * The sketch reports on the Serial monitor the standard deviation of the recorded raw integer
 * data for each channel.
 * 
 * If an SD card was inserted, the sketch records one data buffer to files,
 * stored in the tests/ directory of the SD card. You may run the
 * extras/noise.py script on these files to display histograms and to
 * evaluate the noise levels in more detail.
 */ 

#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Blink.h>

// Settings: --------------------------------------------------------

int bits = 12;                   // resolution: 10bit 12bit, or 16bit
uint32_t samplingRate = 100000;  // samples per second and channel in Hertz
int8_t channels0 [] =  {A5, -1, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};  // input pins for ADC0
int8_t channels1 [] =  {A10, -1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1
int averages = 8;
ADC_CONVERSION_SPEED conversionSpeed = ADC_CONVERSION_SPEED::HIGH_SPEED;
ADC_SAMPLING_SPEED samplingSpeed = ADC_SAMPLING_SPEED::HIGH_SPEED;

// ------------------------------------------------------------------------------------------
 
DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)
ContinuousADC aidata(AIBuffer, NAIBuffer);

SDCard sdcard;
SDWriter file(sdcard, aidata);
Blink blink(LED_BUILTIN);


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.unsetScaling();
  aidata.setResolution(bits);
  aidata.setAveraging(averages);
  aidata.setConversionSpeed(conversionSpeed);
  aidata.setSamplingSpeed(samplingSpeed);
  aidata.check();
}


// mean and standard deviation over first m samples of buffer of size n from channel c
void stdev(float &mean, float &std, uint8_t c, size_t m, size_t n) {
  sample_t buffer[n];
  aidata.getData(c, 0, buffer, n);
  float a = 0.0;
  for (size_t k=0; k<m; k++)
    a += (float(buffer[k]) - a ) / float(k+1);
  mean = a;
  float v = 0.0;
  float ep = 0.0;
  for (size_t k=0; k<m; k++) {
    float s = float(buffer[k]) - a;
    v += s*s;
    ep += s;
  }
  v = (v - ep*ep/m)/(m-1);
  std = sqrt(v);
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  setupADC();
  sdcard.begin();
  file.dataDir("tests");
  delay(500);
}


void loop() {
  char fname[30];
  const char *convs = aidata.conversionSpeedShortStr();
  const char *sampls = aidata.samplingSpeedShortStr();
  Serial.printf("%s conversion speed, %s sampling speed, %d averaging", convs, sampls, aidata.averaging());
  blink.switchOn();
  delay(100);
  blink.switchOff();
  sprintf(fname, "zero-%03.0fkHz-%02dbit-conv%s-sampl%s-avrg%02d.wav", 0.001*aidata.rate(),
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
  file.openWave(fname, 0);
  if ( file.isOpen() ) {
    file.write();
    file.closeWave();
    Serial.printf(" -> saved to %s\n", fname);
    delay(1000);
  }
  else
    Serial.println();
  // analyze:
  size_t nframes = aidata.nbuffer()/aidata.nchannels();
  for (uint8_t c=0; channels0[c] >= 0; c++) {
    char cs[10];
    aidata.channelStr(channels0[c], cs);
    float mean;
    float std;
    stdev(mean, std, 2*c, nframes/2, nframes);
    Serial.printf("%3s: a=%5.1f  a=%5.1f\n", cs, mean, std);
    aidata.channelStr(channels1[c], cs);
    stdev(mean, std, 2*c+1, nframes/2, nframes);
    Serial.printf("%3s: a=%5.1f  a=%5.1f\n", cs, mean, std);
  }
  Serial.println();
}
 
