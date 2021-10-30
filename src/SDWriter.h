/*
  SDWriter - library for writing recorded data to SD card.
  Created by Jan Benda, May 26th, 2021.

  Install SdFat library from Tools->Manage Libraries (search for SdFat).

  By default, the builtin SD card slot is used.

  By definig SD_CONFIG, you can select and configure another slot.
  Something along these lines *before* you include SDWriter.h:
  ```
  // Definition of the chip select pin used for the SD-card.
  #ifndef SDCARD_SS_PIN
    const uint8_t SD_CS_PIN = SS;
  #else 
    // assume built-in SD is used:
    const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
  #endif
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(50))
  ```
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <ContinuousADC.h>
#include <WaveHeader.h>

#define SDCARD_USE_SDFAT

#ifdef SDCARD_USE_SDFAT
  // Use Greimanns SdFat library:
  #include <SdFat.h>

  #define SD_FAT_TYPE 3

  #ifndef SD_CONFIG
    #ifdef BUILTIN_SDCARD
      #define SD_CONFIG BUILTIN_SDCARD
    #else
      #define SD_CONFIG SdioConfig(FIFO_SDIO)
    #endif
  #endif
#else
  // Use Teensy SD library:
  #include <SD.h>
  #include <SPI.h>
  #define SD_CONFIG BUILTIN_SDCARD
  #define FsFile File
#endif


class SDCard {

 public:

  // Initialize SD card.
  SDCard();
  // End usage of SD card.
  ~SDCard();

  // Availability of a SD card. 
  bool available() { return Available; };

  // End usage of SD card.
  void end();

  // Make directory if it does not exist and make it the currrent working directory.
  void dataDir(const char *path);

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Remove all files in path (non-recursively).
  void removeFiles(const char *path);

  // Open file on SD card.
  FsFile open(const char *path, oflag_t oflag=0) { return SD.open(path, oflag); };

  
 protected:

#ifdef SDCARD_USE_SDFAT
  SdFs SD;    // Lydia: SdFatSdio SD; // do not use SdFatSdioEX
#endif
  bool Available;

  uint16_t NameCounter;
};


class SDWriter : public DataConsumer {

 public:

  // Initialize writer on default SD card.
  SDWriter(const DataBuffer &data);
  // Initialize writer on SD card.
  SDWriter(SDCard &sd, const DataBuffer &data);
  // Close file and end usage of SD card.
  ~SDWriter();

  // Availability of a SD card. 
  bool available();

  // End usage of SD card if it was created by SDWriter.
  void end();

  // Make directory if it does not exist and make it the currrent working directory.
  void dataDir(const char *path);

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Set write interval depending on adc settings.
  // Call this *after* setting up ContinusADC in setup().
  void setWriteInterval(const ContinuousADC &adc);

  // True if data need to be written to file.
  // Check this regularly in loop().
  bool needToWrite();

  // Open new file for writing (<=11ms).
  // fname is the name of the file inclusively extension.
  bool open(const char *fname);

  // True if file is open.
  bool isOpen() const;

  // Close file (<=6ms).
  void close();

  // Return file object.
  FsFile &file();


  // Open new file for writing and write wave header for settings from adc.
  // For samples<0, take max file size from adc.
  // For samples=0, initialize wave header with unspecified size.
  // You then need to close the file with closeWave() and provide the number of samples there.
  // If no file extension is provided, ".wav" is added.
  // Takes about <=25ms.
  void openWave(const char *fname, const ContinuousADC &adc, int32_t samples=-1,
                char *datetime=0);

  // Update wave header with proper file size and close file.
  // Takes about <=8ms.
  void closeWave();

  
  // Write available data to file (if the file is open).
  // If maxFileSamples() is set (>0), then stop writing after that many samples. 
  // Returns number of written samples.
  size_t writeData();

  // Start writing to a file from the current data head on.
  void startWrite();

  // Return current file size in samples.
  size_t fileSamples() const;

  // Return current file size in seconds.
  float fileTime() const;

  // Return current file size as a string displaying minutes and seconds.
  // str must hold at least 6 characters.
  void fileTimeStr(char *str) const;

  // Set maximum file size to a fixed number of samples modulo 256.
  void setMaxFileSamples(size_t samples);

  // Set maximum file size to approximately that many seconds.
  void setMaxFileTime(float secs);

  // Return actually used maximum file size in samples.
  size_t maxFileSamples() const;

  // Return true if maximum number of samples have been written
  // and a new file needs to be opened.
  bool endWrite();
  
  // Data buffer has been initialized.
  virtual void reset();
  

 protected:

  SDCard *SD;
  bool SDOwn;
  mutable FsFile DataFile;   // mutable because File from FS.h has non-constant bool() function

  WaveHeader Wave;

  elapsedMillis WriteTime;
  uint WriteInterval;

  size_t FileSamples;    // current number of samples stored in the file.
  size_t FileMaxSamples; // maximum number of samples to be stored in a file.
  
};


#endif
