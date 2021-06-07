#include <WaveFile.h>


WaveFile::WaveFile() :
  Riff("RIFF", "WAVE"),
  Format(),
  Info("LIST", "INFO"),
  Bits("BITS", "16"),
  DateTime("DTIM", ""),
  Software("ISFT", "TeeRec"),
  Data() {
  DataResolution = 16;
  NBuffer = 0;
  Buffer = 0;
}


WaveFile::~WaveFile() {
  if (Buffer != 0)
    delete [] Buffer;
}


WaveFile::Chunk::Chunk(const char *id, uint32_t size) {
  strncpy(Header.Id, id, 4);
  setSize(size);
  NBuffer = sizeof(Header) + Header.Size;
  Buffer = (char *)&Header;
  Use = true;
}


void WaveFile::Chunk::setSize(uint32_t size) {
  Header.Size = ((size+1) >> 1) << 1; // even size
}


void WaveFile::Chunk::addSize(uint32_t size) {
  Header.Size += size;
}


WaveFile::ListChunk::ListChunk(const char *id, const char *listid) :
  Chunk(id, 4) {
  strncpy(ListId, listid, 4);
}


WaveFile::FormatChunk::FormatChunk() :
  Chunk("fmt ", sizeof(Format)) {
  memset(&Format, 0, sizeof(Format));
}


WaveFile::FormatChunk::FormatChunk(uint8_t nchannels, uint32_t samplerate,
				   uint16_t resolution) :
  Chunk("fmt ", sizeof(Format)) {
  set(nchannels, samplerate, resolution);
}


void WaveFile::FormatChunk::set(uint8_t nchannels, uint32_t samplerate,
			        uint16_t resolution) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Format.formatTag = 1;                   // 1 is PCM
  Format.numChannels = nchannels;
  Format.sampleRate = samplerate;
  Format.byteRate = samplerate * nchannels * nbytes;
  Format.blockAlign = nchannels * nbytes;
  Format.bitsPerSample = resolution;
}


WaveFile::InfoChunk::InfoChunk(const char *infoid, const char *text) :
  Chunk(infoid, 0) {
  set(text);
}


void WaveFile::InfoChunk::set(const char *text) {
  setSize(strlen(text));
  NBuffer = sizeof(Header) + Header.Size;
  strncpy(Text, text, MaxText);
  Use = (strlen(text) > 0);
}


void WaveFile::InfoChunk::clear() {
  setSize(0);
  Use = false;
}


WaveFile::DataChunk::DataChunk() :
  Chunk("data", 0) {
}


WaveFile::DataChunk::DataChunk(uint16_t resolution, int32_t samples) :
  Chunk("data", 0) {
  set(resolution, samples);
}


void WaveFile::DataChunk::set(uint16_t resolution, int32_t samples) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Header.Size = samples * nbytes;         // in bytes, nchannels is already in samples
}


void WaveFile::setFormat(uint8_t nchannels, uint32_t samplerate,
		         uint16_t resolution, uint16_t dataresolution) {
  Format.set(nchannels, samplerate, dataresolution);
  DataResolution = dataresolution;
  char bs[4];
  sprintf(bs, "%d", resolution);
  Bits.set(bs);
}


void WaveFile::setData(int32_t samples) {
  Data.set(DataResolution, samples);
}


void WaveFile::setDateTime(const char *datetime) {
  DateTime.set(datetime);
}


void WaveFile::clearDateTime() {
  DateTime.clear();
}


void WaveFile::setSoftware(const char *software) {
  Software.set(software);
}


void WaveFile::assemble() {
  if (Buffer != 0)
    delete [] Buffer;
  // riff chunks:
  int nchunks = 0;
  Chunk *chunks[10];
  chunks[nchunks++] = &Riff;
  chunks[nchunks++] = &Format;
  chunks[nchunks++] = &Info;
  chunks[nchunks++] = &Bits;
  if (DateTime.Use)
    chunks[nchunks++] = &DateTime;
  if (Software.Use)
    chunks[nchunks++] = &Software;
  chunks[nchunks++] = &Data;
  // update file size:
  Riff.Header.Size = 4;
  for (int k=1; k<nchunks; k++)
    Riff.Header.Size += chunks[k]->NBuffer;
  NBuffer = 8 + Riff.Header.Size;
  Riff.Header.Size += Data.Header.Size;
  // update info size:
  Info.Header.Size = 4;
  for (int k=3; k<nchunks-1; k++)
    Info.addSize(chunks[k]->NBuffer);
  // make header:
  Buffer = new char[NBuffer];
  uint32_t idx = 0;
  for (int k=0; k<nchunks; k++) {
    memcpy(&Buffer[idx], chunks[k]->Buffer, chunks[k]->NBuffer);
    idx += chunks[k]->NBuffer;
  }
}

