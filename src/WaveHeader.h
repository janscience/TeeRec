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

  static const size_t MaxBuffer = 512;
  size_t NBuffer;
  char Buffer[MaxBuffer];

  // Set parameters of the format chunk.
  void setFormat(uint8_t nchannels, uint32_t samplerate,
		 uint16_t resolution, uint16_t dataresolution);

  // Return string describing analog input pins.
  const char *channels() const { return Channels.text(); };

  // Set analog input pins.
  void setChannels(const char *chans);

  // Return string describing number of averages per sample.
  const char *averaging() const { return Averaging.text(); };

  // Set number of averages per sample.
  void setAveraging(uint8_t num);

  // Return string describing conversion speed.
  const char *conversionSpeed() const { return Conversion.text(); };

  // Set string describing conversion speed.
  void setConversionSpeed(const char *conversion);

  // Return string describing sampling speed.
  const char *samplingSpeed() const { return Sampling.text(); };

  // Set string describing sampling speed.
  void setSamplingSpeed(const char *sampling);

  // Return string describing voltage reference.
  const char *reference() const { return Reference.text(); };

  // Set string describing voltage reference.
  void setReference(const char *ref);

  // Return string describing overall gain.
  const char *gain() const { return Gain.text(); };

  // Set string describing overall gain.
  void setGain(const char *gain);

  // Set size of data chunk.
  // Call this *after* setFormat().
  void setData(int32_t samples=0);

  // Return string describing start time of recording.
  const char *dateTime() const { return DateTime.text(); };

  // Set date and time of recording.
  void setDateTime(const char *datetime);

  // Clear date and time of recording.
  void clearDateTime();

  // Return string describing software.
  const char *software() const { return Software.text(); };

  // Set name of software.
  void setSoftware(const char *software);

  // Clear name of software.
  void clearSoftware();

  // Return string describing CPU speed.
  const char *cpuSpeed() const { return CPUSpeed.text(); };

  // Set CPU speed to current CPU speed.
  void setCPUSpeed();

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
  template <size_t N=32> class InfoChunk : public Chunk {
  public:
    InfoChunk(const char *infoid, const char *text);
    const char *text() const { return Text; };
    void set(const char *text);
    void clear();
    static const size_t MaxText = N;
    char Text[N];
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
  InfoChunk<4> Bits;
  InfoChunk<4> DataBits;
  InfoChunk<256> Channels;
  InfoChunk<4> Averaging;
  InfoChunk<32> Conversion;
  InfoChunk<32> Sampling;
  InfoChunk<8> Reference;
  InfoChunk<16> Gain;
  InfoChunk<16> Board;
  InfoChunk<32> MAC;
  InfoChunk<8> CPUSpeed;
  InfoChunk<32> DateTime;
  InfoChunk<64> Software;
  DataChunk Data;

};


#endif
