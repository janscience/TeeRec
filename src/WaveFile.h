/*
  WaveFile - library for writing wave files with metadata.
  Created by Jan Benda, June 5, 2021.
*/

#ifndef WaveFile_h
#define WaveFile_h


#include <Arduino.h>


class ContinuousADC;
class SDWriter;
class FsFile;


class WaveFile {

 public:

  // Open new file for writing and write wave header for settings from adcc.
  // For samples<0, take max file size from adc.
  // For samples=1, initialize wave header with unsepcified size.
  // You then need to close the file with closeWave() and provide the number of samples there.
  // If no file extension is provided, ".wav" is added.
  void open(SDWriter &file, const char *fname, const ContinuousADC &adc, int32_t samples=-1);

  // Update wave header with proper file size and close file.
  // If you supplied the right number of samples already to openWave(), 
  // then it is sufficient to simply close() the file.
  // Takes about 5ms.
  void close(SDWriter &file, const ContinuousADC &adc, uint32_t samples);


 protected:

  typedef struct {
    char Id[4];        // 4 character chunck ID.
    uint32_t Size;     // size of chunk in bytes
  } Chunk;

  class ChunkBuffer {
  public:
    ChunkBuffer();
    ChunkBuffer(const char *id, uint32_t size);
    ~ChunkBuffer();
    void addSize(uint32_t size);
    Chunk *Header;
    char *Data;
    char *Buffer;
    uint32_t NBuffer;
  };

  class RiffChunk : public ChunkBuffer {
  public:
    RiffChunk();
  };

  class FormatChunk : public ChunkBuffer {

    typedef struct {
      uint16_t formatTag;        // 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM
      uint16_t numChannels;      // number of channels/pins used
      uint32_t sampleRate;       // sampling rate in samples per second
      uint32_t byteRate;         // byteRate = sampleRate * numChannels * bitsPerSample/8
      uint16_t blockAlign;       // number of bytes per frame
      uint16_t bitsPerSample;    // number of bits per sample
    } Format_t;

  public:
    FormatChunk(uint8_t nchannels, uint32_t samplerate,
		uint16_t resolution);
    Format_t *Format;
  };

  class DataChunk : public ChunkBuffer {
  public:
    DataChunk(uint16_t resolution, int32_t samples);
  };

  // Write wave file header to file.
  // If samples=0, you need to supply the number of samples to closeWave().
  void writeHeader(FsFile &file, uint8_t nchannels, uint32_t samplerate,
	           uint16_t resolution, int32_t samples=0);

};


#endif
