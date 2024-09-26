// select SD card:
#define SDCARD_BUILTIN
//#define SDCARD_SPI0
//#define SDCARD_SPI1

#include <SDCard.h>
#include <TeeRecBanner.h>

SDCard sdcard;


void pause() {
  while (Serial.available() == 0)
    yield();
  char pval[8];
  Serial.readBytesUntil('\n', pval, 8);
}


bool yesno(const char *request, bool defval) {
  while (true) {
    Serial.print(request);
    if (defval)
      Serial.print(" [Y/n] ");
    else
      Serial.print(" [y/N] ");
    while (Serial.available() == 0)
      yield();
    char pval[8];
    Serial.readBytesUntil('\n', pval, 8);
    Serial.println(pval);
    if (strlen(pval) == 0)
      return defval;
    if (tolower(pval[0]) == 'y')
      return true;
    if (tolower(pval[0]) == 'n')
      return false;
  }
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  printTeeRecBanner();
}


void loop() {
  // menu:
  Serial.println("1) SD card info");
  Serial.println("2) SD card check");
  Serial.println("3) SD card benchmark test");
  Serial.println("4) SD card format");
  Serial.println("5) SD card erase and format");
  Serial.println("6) SD card list files");
  Serial.println("7) SD card remove files");
  Serial.println();
  Serial.print("Please select an action: ");
  // clear serial input:
  uint32_t m = micros();
  do {
    if (Serial.read() >= 0)
      m = micros();
  } while (micros() - m < 10000);
  // read input:
  while (Serial.available() == 0)
    yield();
  char pval[32];
  Serial.readBytesUntil('\n', pval, 32);
  Serial.println(pval);
  Serial.println();
  if (strlen(pval) != 1 || pval[0] < '1' || pval[0] > '7')
    return;
  // open SD card:
#if defined(SDCARD_BUILTIN)
  sdcard.begin();
#elif defined(SDCARD_SPI0)
  sdcard.begin(10, DEDICATED_SPI, 20, &SPI);
#elif defined(SDCARD_SPI1)
  sdcard.begin(0, DEDICATED_SPI, 20, &SPI1);
#endif
  // run action:
  if (pval[0] == '1') {
    sdcard.report();
  }
  else if (pval[0] == '2') {
    sdcard.check();
  }
  else if (pval[0] == '3') {
    sdcard.benchmark();
  }
  else if (pval[0] == '4') {
    if (yesno("Do you really want to format the SD card?", false)) {
      Serial.println();
      sdcard.format("", false);
    }
    else
      Serial.println();
  }
  else if (pval[0] == '5') {
    if (yesno("Do you really want to erase and format the SD card?", false)) {
      Serial.println();
      sdcard.format("", true);
    }
    else
      Serial.println();
  }
  else if (pval[0] == '6') {
    sdcard.listFiles("/", true);
    Serial.println();
  }
  else if (pval[0] == '7') {
    if (yesno("Do you really want to remove all files from the SD card?", false)) {
      Serial.println();
      sdcard.removeFiles("/");
    }
    Serial.println();
  }
  pause();
  // close SD card:
  sdcard.end();
}
