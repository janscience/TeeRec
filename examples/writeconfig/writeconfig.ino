// select a data source:
#define TEENSYADC     // data are recorded from Teensy internal ADCs
//#define PCM186X     // data are recorded by TI PCM186x chip via TDM

#include <RTClock.h>
#include <SDWriter.h>
#include <Configurator.h>
#include <ToolActions.h>
#include <Settings.h>
#if defined(TEENSYADC)
  #include <InputADC.h>
  #include <InputADCSettings.h>
#elif defined(PCM186X)
  #include <InputTDMSettings.h>
#endif
#include <Blink.h>

// Adapt the following parameter values to your needs:

#define CFG_FILE        "teerec.cfg"   // name of configuration file

// Settings:
#define PATH            "recordings" // folder where to store the recordings
#define FILENAME        "rec1-NUM4.wav"  // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
#define FILE_SAVE_TIME  10   // seconds
#define INITIAL_DELAY   10.0  // seconds
#define PULSE_FREQUENCY 230 // Hertz

// Input XXXSettings:
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


RTClock rtclock;
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
Configurable datetime_menu("Date & time", Action::StreamInput);
ReportRTCAction report_rtc_act(datetime_menu, "Print date & time", rtclock);
SetRTCAction set_rtc_act(datetime_menu, "Set date & time", rtclock);
Configurable config_menu("Configuration", Action::StreamInput);
ReportConfigAction report_act(config_menu, "Print configuration");
SaveConfigAction save_act(config_menu,"Save configuration", sdcard);
LoadConfigAction load_act(config_menu, "Load configuration", sdcard);
RemoveConfigAction remove_act(config_menu, "Erase configuration", sdcard);
Configurable sdcard_menu("SD card", Action::StreamInput);
SDInfoAction sdinfo_act(sdcard_menu, "SD card info", sdcard);
SDFormatAction format_act(sdcard_menu, "Format SD card", sdcard);
SDListAction list_act(sdcard_menu, "List all recordings", sdcard, settings);
SDRemoveAction erase_act(sdcard_menu, "Erase all recordings", sdcard, settings);

Blink blink(LED_BUILTIN);


void setup() {
  // disable parameter that should not go into the configuration file here:
  settings.disable("PulseFreq");
  settings.disable("DisplayTime");
  settings.disable("SensorsInterval");

  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  Serial.println();
  blink.switchOn();
  sdcard.begin();
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
