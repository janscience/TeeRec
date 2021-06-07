/*
  SDWriter - library for writing recorded data to SD card.
  Created by Jan Benda, May 26th, 2021.

  Install SdFat library from Tools->Manage Libraries (search for SdFat).
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <WaveFile.h>
#include <SdFat.h>

#define SD_FAT_TYPE 3

// Definition of the pin used for the SD-card.
// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
  const uint8_t SD_CS_PIN = SS;
#else 
  // assume built-in SD is used:
  const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif

// Try to select the best SD card configuration:
#if HAS_SDIO_CLASS
  #define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI)
#else
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI)
#endif


class ContinuousADC;


class SDWriter {

 public:

  // Initialize SD card.
  SDWriter();
  // End usage of SD card.
  ~SDWriter();

  // Availability of a SD card. 
  bool available();

  // End usage of SD card.
  void end();

  // Make directory if it does not exist and make it the currrent working directory.
  void dataDir(const char *path);

  // Set write interval depending on adc settings.
  // Call this *after* setting up ContinusADC in setup().
  void setWriteInterval(const ContinuousADC &adc);

  // True if data need to be written to file.
  // Check this regularly in loop().
  bool needToWrite();

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Open new file for writing.
  // fname is the name of the file inclusively extension.
  bool open(const char *fname);

  // True if file is open.
  bool isOpen() const;

  // Close file.
  void close();

  // Return file object.
  FsFile &file();


  // Open new file for writing and write wave header for settings from adc.
  // For samples<0, take max file size from adc.
  // For samples=0, initialize wave header with unspecified size.
  // You then need to close the file with closeWave() and provide the number of samples there.
  // If no file extension is provided, ".wav" is added.
  void openWave(const char *fname, const ContinuousADC &adc, int32_t samples=-1,
                char *datetime=0);

  // Update wave header with proper file size and close file.
  // If you supplied the right number of samples already to openWave(), 
  // then it is sufficient to simply close() the file.
  // Takes about 5ms.
  void closeWave(uint32_t samples);


 protected:

  SdFs SD;    // Lydia: SdFatSdio SD;
  bool SDAvailable;
  FsFile File;

  WaveFile Wave;

  elapsedMillis WriteTime;
  uint WriteInterval;

  uint16_t NameCounter;

};


#endif
