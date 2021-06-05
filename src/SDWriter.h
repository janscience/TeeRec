/*
  SDWriter - library for writing recorded data to SD card.
  Created by Jan Benda, May 26th, 2021.

  Install SdFat library from Tools->Manage Libraries (search for SdFat).
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
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
  void open(const char *fname);

  // True if file is open.
  bool isOpen() const;

  // Close file.
  void close();

  // Return file object.
  FsFile &file();

  // Open new file for writing and write wave header for settings from adcc.
  // For samples<0, take max file size from adc.
  // For samples=1, initialize wave header with unsepcified size.
  // You then need to close the file with closeWave() and provide the number of samples there.
  // If no file extension is provided, ".wav" is added.
  void openWave(const char *fname, const ContinuousADC &adc, int32_t samples=-1);

  // Update wave header with proper file size and close file.
  // If you supplied the right number of samples already to openWave(), 
  // then it is sufficient to simply close() the file.
  // Takes about 5ms.
  void closeWave(const ContinuousADC &adc, uint32_t samples);


 protected:

  SdFs SD;    // Lydia: SdFatSdio SD;
  bool SDAvailable;
  FsFile File;

  elapsedMillis WriteTime;
  uint WriteInterval;

  uint16_t NameCounter;

  // WAVE header:
  typedef struct {
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

  // Write wave file header to file.
  // If samples=0, you need to supply the number of samples to closeWave().
  void writeWaveHeader(uint8_t nchannels, uint32_t samplerate,
		       uint16_t resolution, int32_t samples=0);


};


#endif
