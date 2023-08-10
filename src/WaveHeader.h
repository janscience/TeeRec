/*
  WaveHeader - library for setting up wave file header with metadata.
  Created by Jan Benda, June 5, 2021.
*/

#ifndef WaveHeader_h
#define WaveHeader_h


#include <Arduino.h>


class WaveHeader {

public:

  WaveHeader();
  ~WaveHeader();

  static const size_t MaxBuffer = 500;
  size_t NBuffer;
  char Buffer[MaxBuffer];

  // Set parameters of the format chunk.
  void setFormat(uint8_t nchannels, uint32_t samplerate,
		 uint16_t resolution, uint16_t dataresolution);

  // Set analog input pins.
  void setChannels(const char *chans);

  // Set number of averages.
  void setAveraging(uint8_t num);

  // Set string describing conversion speed.
  void setConversionSpeed(const char *conversion);

  // Set string describing sampling speed.
  void setSamplingSpeed(const char *sampling);

  // Set string describing voltage reference.
  void setReference(const char *ref);

  // Set string describing overall gain.
  void setGain(const char *gain);

  // Set size of data chunk.
  // Call this *after* setFormat().
  void setData(int32_t samples=0);

  // Set date and time of recording.
  void setDateTime(const char *datetime);

  // Clear date and time of recording.
  void clearDateTime();

  // Set name of software.
  void setSoftware(const char *software);

  // Clear name of software.
  void clearSoftware();

  // Assemble wave header from previously set infos.
  // The header can then be retrieved from Buffer.
  void assemble();


protected:

  typedef struct {
    char Id[4];        // 4 character chunck ID.
    uint32_t Size;     // size of chunk in bytes
  } ChunkHead;

  class Chunk {
  public:
    Chunk(const char *id, uint32_t size);
    void setSize(uint32_t size);
    void addSize(uint32_t size);
    char *Buffer;
    uint32_t NBuffer;
    bool Use;
    ChunkHead Header;
  };

  class ListChunk : public Chunk {
  public:
    ListChunk(const char *id, const char *listid);
    char ListId[4];
  };

  class FormatChunk : public Chunk {

    typedef struct {
      uint16_t formatTag;        // 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM
      uint16_t numChannels;      // number of channels/pins used
      uint32_t sampleRate;       // sampling rate in samples per second
      uint32_t byteRate;         // byteRate = sampleRate * numChannels * bitsPerSample/8
      uint16_t blockAlign;       // number of bytes per frame
      uint16_t bitsPerSample;    // number of bits per sample
    } Format_t;

  public:
    FormatChunk();
    FormatChunk(uint8_t nchannels, uint32_t samplerate,
		uint16_t resolution);
    void set(uint8_t nchannels, uint32_t samplerate, uint16_t resolution);
    Format_t Format;
  };

  // See https://exiftool.org/TagNames/RIFF.html#Info for valid info tags.
  class InfoChunk : public Chunk {
  public:
    InfoChunk(const char *infoid, const char *text);
    void set(const char *text);
    void clear();
    static const size_t MaxText = 127;
    char Text[MaxText+1];
  };

  class DataChunk : public Chunk {
  public:
    DataChunk();
    DataChunk(uint16_t resolution, int32_t samples);
    void set(uint16_t resolution, int32_t samples);
  };

  uint16_t DataResolution;

  ListChunk Riff;
  FormatChunk Format;
  ListChunk Info;
  InfoChunk Bits;
  InfoChunk DataBits;
  InfoChunk Channels;
  InfoChunk Averaging;
  InfoChunk Conversion;
  InfoChunk Sampling;
  InfoChunk Reference;
  InfoChunk Gain;
  InfoChunk Board;
  InfoChunk MAC;
  InfoChunk DateTime;
  InfoChunk Software;
  DataChunk Data;

};


#endif
