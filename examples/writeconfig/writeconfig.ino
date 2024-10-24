// select a data source:
//#define INPUT_ADC     // data are recorded from Teensy internal ADCs
#define INPUT_TDM     // data are recorded by TI PCM186x chip via TDM

// select SD card:
#define SDCARD_BUILTIN
//#define SDCARD_SPI0
//#define SDCARD_SPI1

#include <RTClock.h>
#include <SDCard.h>
#include <Configurator.h>
#include <ToolActions.h>
#include <ToolMenus.h>
#include <Settings.h>
#if defined(INPUT_ADC)
  #include <InputADC.h>
  #include <InputADCSettings.h>
#elif defined(INPUT_TDM)
  #include <InputTDMSettings.h>
#endif
#include <Blink.h>
#include <TeeRecBanner.h>

// Adapt the following parameter values to your needs:

#define CFG_FILE        "teerec.cfg"   // name of configuration file

// Settings:
#define PATH            "recordings"   // folder where to store the recordings
#define DEVICEID        0                 // device identifier
#define FILENAME        "recID-NUM4.wav"  // may include ID, IDA, DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
#define FILE_SAVE_TIME  10   // seconds
#define INITIAL_DELAY   10.0  // seconds
#define PULSE_FREQUENCY 230 // Hertz

// Input XXXSettings:
#define SAMPLING_RATE  48000 // samples per second and channel in Hertz
#if defined(INPUT_ADC)
  #define BITS           12    // resolution: 10bit 12bit, or 16bit
  #define AVERAGING       8    // number of averages per sample: 0, 4, 8, 16, 32
  #define CONVERSION     ADC_CONVERSION_SPEED::HIGH_SPEED
  #define SAMPLING       ADC_SAMPLING_SPEED::HIGH_SPEED
  #define REFERENCE      ADC_REFERENCE::REF_3V3
#elif defined(INPUT_TDM)
  #define GAIN           20.0  // dB
#endif


RTClock rtclock;
SDCard sdcard;

Configurator config;
Settings settings(PATH, DEVICEID, FILENAME, FILE_SAVE_TIME, PULSE_FREQUENCY,
                  0.0, INITIAL_DELAY);
#if defined(INPUT_ADC)
InputADCSettings aisettings(SAMPLING_RATE, BITS, AVERAGING,
		  	    CONVERSION, SAMPLING, REFERENCE);
#elif defined(INPUT_TDM)
InputTDMSettings aisettings(SAMPLING_RATE, 8, GAIN);
#endif
DateTimeMenu datetime_menu(rtclock);
ConfigurationMenu configuration_menu(sdcard);
SDCardMenu sdcard0_menu("SD card", sdcard, settings);
#ifdef FIRMWARE_UPDATE
FirmwareMenu firmware_menu(sdcard);
#endif
DiagnosticMenu diagnostic_menu("Diagnostics", sdcard);
HelpAction help_act(config, "Help");

Blink blink(LED_BUILTIN);


void setup() {
  // disable parameter that should not go into the configuration file here:
  settings.disable("PulseFreq");
  settings.disable("DisplayTime");
  settings.disable("SensorsInterval");

  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  printTeeRecBanner();
  blink.switchOn();
#if defined(SDCARD_BUILTIN)
  sdcard.begin();
#elif defined(SDCARD_SPI0)
  pinMode(10, OUTPUT);
  SPI.begin();
  sdcard.begin(10, DEDICATED_SPI, 40, &SPI);
#elif defined(SDCARD_SPI1)
  pinMode(0, OUTPUT);
  SPI.begin();
  sdcard.begin(0, DEDICATED_SPI, 40, &SPI1);
#endif
  sdcard.check();
  config.setConfigFile(CFG_FILE);
  config.load(sdcard);
  if (Serial)
    config.configure(Serial, 10000);
  config.report();
  Serial.println();
  blink.switchOff();
  sdcard.end();
}


void loop() {
  delay(100);
}
