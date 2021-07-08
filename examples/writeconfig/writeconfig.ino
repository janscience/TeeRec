#include <SDWriter.h>
#include <Blink.h>

const char *config = R"""(# Configuration file for TeeRec based logger.

Settings:
  Path       : recordings  # path where to store data
  FileName   : rec-SDATETIME.wav  # may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
  FileTime   : 10min       # s or min
  DisplayTime: 5ms         # ms only
  PulseFreq  : 200Hz       # Hz or kHz

ADC:
  SamplingRate: 44.1kHz    # Hz or kHz
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
  FsFile file = sd.open(fname, O_WRITE | O_CREAT);
  size_t n = strlen(config);
  size_t m = file.write(config, n);
  if (n != m)
    Serial.println("Failed to write config file.");
  else {
    Serial.printf("Wrote configuration file \"%s\".\n", fname);
    Serial.println();
    Serial.println("Open this file in your favourite editor and adapt it to your needs.");
  }
  file.close();
  blink.switchOff();
  sd.end();
}


void loop() {
}
