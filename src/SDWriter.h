/*
  SDWriter - library for writing recorded data to SD card.
  Created by Jan Benda, May 26th, 2021.

  Install SdFat library from Tools->Manage Libraries (search for SdFat).
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <ContinuousADC.h>
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

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Open new file for writing.
  // fname is the name of the file inclusively extension.
  void open(const char *fname);

  // Close file.
  void close();

  // Return file object.
  FsFile &file();

  // Initialize wave file header.
  // If samples=0, you need to supply the number of samples to closeWave().
  void setupWaveHeader(uint8_t nchannels, uint32_t samplerate,
		       uint16_t resolution, int32_t samples=0);
  // Initialize wave file header from adc settings.
  // If samples=0, you need to supply the number of samples to closeWave().
  // If samples is negative, then adcc.maxFileSamples() is used.
  void setupWaveHeader(const ContinuousADC &adcc, int32_t samples=-1);

  // Open new file for writing and write wave header.
  // You need to setup the wave header before!
  // If no file extension is provided, ".wav" is added.
  // Takes about 1ms.
  void openWave(const char *fname);

  // Close wave file and update header with file size.
  // Takes about 5ms.
  void closeWave(uint32_t samples=0);


 protected:

  SdFs SD;
  bool SDAvailable;
  FsFile File;

  uint16_t NameCounter;

  // WAVE header:
  struct fileheader {
    char mainChunkId[4];       // "RIFF"
    uint32_t mainChunkSize;    // file length in bytes
    char mainChunkFormat[4];   // "WAVE"
    char fmtChunkId[4];        // "fmt "
    uint32_t fmtChunkSize;     // size of FMT chunk in bytes (usually 16 for PCM)
    uint16_t formatTag;        // 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM
    uint16_t numChannels;      // number of channels/pins used
    uint32_t sampleRate;       // sampling rate in samples per second
    uint32_t byteRate;         // byteRate = sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign;       // number of bytes per frame
    uint16_t bitsPerSample;    // number of bits per sample
    char SubtwoChunkId[4];     // "data"
    uint32_t SubtwoChunkSize;  // data length in bytes (filelength - 44)
  } WaveHeader;

  uint8_t NChannels;
  uint8_t NBytes;

};


#endif
