#include <SDWriter.h>
#include <Blink.h>

const char *config = R"""(# Configuration file for TeeRec based logger.

Settings:
  Path       : recordings  # path where to store data
  FileName   : logger1-SDATETIME.wav  # may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
  FileTime   : 10min       # s, min, or h
  PulseFreq  : 200Hz       # Hz, kHz, MHz
  DisplayTime: 5ms         # ms, s

ADC:
  SamplingRate: 44.1kHz    # Hz, kHz, MHz
  Averaging   : 4
  Conversion  : high
  Sampling    : high
  Resolution  : 12bit
  Reference   : 3.3V
)""";

const char *fname = "teerec.cfg";


SDCard sd;
Blink blink;


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  blink.switchOn();
  sd.begin();
  FsFile file = sd.openWrite(fname);
  size_t n = strlen(config);
  size_t m = file.write(config, n);
  if (n != m)
    Serial.println("Failed to write config file.");
  else {
    Serial.printf("Wrote configuration file \"%s\".\n", fname);
    Serial.println();
    Serial.println("Rename and edit this file to your needs.");
  }
  file.close();
  blink.switchOff();
  sd.end();
}


void loop() {
}
