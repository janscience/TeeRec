// select a data source:
//#define TEENSYADC     // data are recorded from Teensy internal ADCs
#define PCM186X     // data are recorded by TI PCM186x chip via TDM

#include <SDWriter.h>
#include <Configurator.h>
#include <Settings.h>
#if defined(TEENSYADC)
#include <InputADC.h>
#include <InputADCSettings.h>
#elif defined(PCM186X)
#include <InputTDMSettings.h>
#endif
#include <Blink.h>

#define SAMPLING_RATE  48000 // samples per second and channel in Hertz
#if defined(TEENSYADC)
#define BITS           12    // resolution: 10bit 12bit, or 16bit
#define AVERAGING       8    // number of averages per sample: 0, 4, 8, 16, 32
#define CONVERSION     ADC_CONVERSION_SPEED::HIGH_SPEED
#define SAMPLING       ADC_SAMPLING_SPEED::HIGH_SPEED
#define REFERENCE      ADC_REFERENCE::REF_3V3
#elif defined(PCM186X)
#define GAIN           20.0  // dB
#endif

#define PATH          "recordings" // folder where to store the recordings
#define FILENAME      "rec1-NUM4.wav"  // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
#define FILE_SAVE_TIME 10   // seconds
#define INITIAL_DELAY  2.0  // seconds
#define PULSE_FREQUENCY 230 // Hertz

const char *fname = "teerec.cfg";

SDCard sdcard;
Configurator config;
Settings settings(PATH, FILENAME, FILE_SAVE_TIME, PULSE_FREQUENCY,
                  0.0, INITIAL_DELAY);
#if defined(TEENSYADC)
InputADCSettings aisettings(SAMPLING_RATE, BITS, AVERAGING,
		  	    CONVERSION, SAMPLING, REFERENCE);
#elif defined(PCM186X)
InputTDMSettings aisettings(SAMPLING_RATE, 8, GAIN);
#endif
Blink blink(LED_BUILTIN);


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.switchOn();
  sdcard.begin();
  config.setConfigFile(fname);
  config.save(sdcard);
  /*
  if (n != m)
    Serial.println("Failed to write config file.");
  else {
    */
    Serial.printf("Wrote configuration file \"%s\".\n", fname);
    Serial.println();
    Serial.println("Rename and edit this file to your needs.");
  //}
  blink.switchOff();
  sdcard.end();
}


void loop() {
}
