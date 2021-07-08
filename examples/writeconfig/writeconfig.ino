#include <SDWriter.h>
#include <Blink.h>

const char *config = R"""(\
# Configuration file for TeeRec based logger.

SDWriter:
  FileName: logger1-SDATETIME.wav

ContinuousADC:
  SamplingRate: 44.1kHz
  Averaging   : 8
  Conversion  : high
  Sampling    : high
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
