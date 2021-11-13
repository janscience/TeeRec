#include <TeensyBoard.h>
#include <WaveHeader.h>


WaveHeader::WaveHeader() :
  Riff("RIFF", "WAVE"),
  Format(),
  Info("LIST", "INFO"),
  Bits("BITS", "16"),
  DataBits("DBTS", "16"),
  Channels("PINS", ""),
  Averaging("AVRG", ""),
  Conversion("CNVS", ""),
  Sampling("SMPS", ""),
  Reference("VREF", ""),
  Board("IBRD", teensyBoard()),
  MAC("IMAC", teensyMAC()),
  DateTime("DTIM", ""),
  Software("ISFT", "TeeRec"),
  Data() {
  DataResolution = 16;
  NBuffer = 0;
}


WaveHeader::~WaveHeader() {
}


WaveHeader::Chunk::Chunk(const char *id, uint32_t size) {
  strncpy(Header.Id, id, 4);
  setSize(size);
  NBuffer = sizeof(Header) + Header.Size;
  Buffer = (char *)&Header;
  Use = true;
}


void WaveHeader::Chunk::setSize(uint32_t size) {
  Header.Size = ((size+1) >> 1) << 1; // even size needed for wave
}


void WaveHeader::Chunk::addSize(uint32_t size) {
  Header.Size += size;
}


WaveHeader::ListChunk::ListChunk(const char *id, const char *listid) :
  Chunk(id, 4) {
  strncpy(ListId, listid, 4);
}


WaveHeader::FormatChunk::FormatChunk() :
  Chunk("fmt ", sizeof(Format)) {
  memset(&Format, 0, sizeof(Format));
}


WaveHeader::FormatChunk::FormatChunk(uint8_t nchannels, uint32_t samplerate,
				   uint16_t resolution) :
  Chunk("fmt ", sizeof(Format)) {
  set(nchannels, samplerate, resolution);
}


void WaveHeader::FormatChunk::set(uint8_t nchannels, uint32_t samplerate,
			          uint16_t resolution) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Format.formatTag = 1;                   // 1 is PCM
  Format.numChannels = nchannels;
  Format.sampleRate = samplerate;
  Format.byteRate = samplerate * nchannels * nbytes;
  Format.blockAlign = nchannels * nbytes;
  Format.bitsPerSample = nbytes*8;
}


WaveHeader::InfoChunk::InfoChunk(const char *infoid, const char *text) :
  Chunk(infoid, 0) {
  set(text);
}


void WaveHeader::InfoChunk::set(const char *text) {
  setSize(strlen(text));
  NBuffer = sizeof(Header) + Header.Size;
  strncpy(Text, text, MaxText);
  Use = (strlen(text) > 0);
}


void WaveHeader::InfoChunk::clear() {
  setSize(0);
  Use = false;
}


WaveHeader::DataChunk::DataChunk() :
  Chunk("data", 0) {
}


WaveHeader::DataChunk::DataChunk(uint16_t resolution, int32_t samples) :
  Chunk("data", 0) {
  set(resolution, samples);
}


void WaveHeader::DataChunk::set(uint16_t resolution, int32_t samples) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Header.Size = samples * nbytes;         // in bytes, nchannels is already in samples
}


void WaveHeader::setFormat(uint8_t nchannels, uint32_t samplerate,
		           uint16_t resolution, uint16_t dataresolution) {
  Format.set(nchannels, samplerate, dataresolution);
  DataResolution = dataresolution;
  char bs[4];
  sprintf(bs, "%u", resolution);
  Bits.set(bs);
  sprintf(bs, "%u", dataresolution);
  DataBits.set(bs);
  DataBits.Use = (dataresolution != Format.Format.bitsPerSample);
}


void WaveHeader::setChannels(const char *chans) {
  Channels.set(chans);
}


void WaveHeader::setAveraging(uint8_t num) {
  char ns[3];
  sprintf(ns, "%u", num);
  Averaging.set(ns);
}


void WaveHeader::setConversionSpeed(const char *conversion) {
  Conversion.set(conversion);
}


void WaveHeader::setSamplingSpeed(const char *sampling) {
  Sampling.set(sampling);
}


void WaveHeader::setReference(const char *ref) {
  Reference.set(ref);
}


void WaveHeader::setData(int32_t samples) {
  Data.set(DataResolution, samples);
}


void WaveHeader::setDateTime(const char *datetime) {
  DateTime.set(datetime);
}


void WaveHeader::clearDateTime() {
  DateTime.clear();
}


void WaveHeader::setSoftware(const char *software) {
  Software.set(software);
}


void WaveHeader::clearSoftware() {
  Software.clear();
}


void WaveHeader::assemble() {
  // riff chunks:
  const int maxchunks = 15;
  int nchunks = 0;
  Chunk *chunks[maxchunks];
  chunks[nchunks++] = &Riff;
  chunks[nchunks++] = &Format;
  chunks[nchunks++] = &Info;
  chunks[nchunks++] = &Bits;
  if (DataBits.Use)
    chunks[nchunks++] = &DataBits;
  if (Channels.Use)
    chunks[nchunks++] = &Channels;
  if (Averaging.Use)
    chunks[nchunks++] = &Averaging;
  if (Conversion.Use)
    chunks[nchunks++] = &Conversion;
  if (Sampling.Use)
    chunks[nchunks++] = &Sampling;
  if (Reference.Use)
    chunks[nchunks++] = &Reference;
  if (Board.Use)
    chunks[nchunks++] = &Board;
  if (MAC.Use)
    chunks[nchunks++] = &MAC;
  if (DateTime.Use)
    chunks[nchunks++] = &DateTime;
  if (Software.Use)
    chunks[nchunks++] = &Software;
  chunks[nchunks++] = &Data;
  if (nchunks > maxchunks)
    Serial.println("WaveHeader::assemble(): maxchunks too small!\n");
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
  // expand to next multiple of 4
  // (without this we get some unwanted zeros into the file... why?)
  uint32_t infosize = ((Info.Header.Size+3) >> 2) << 2;
  infosize -= Info.Header.Size;
  Info.Header.Size += infosize;
  Riff.Header.Size += infosize;
  NBuffer += infosize;
  // assemble header buffer:
  if (NBuffer > MaxBuffer) {
    NBuffer = 0;
    Serial.println("WaveHeader::assemble(): Header too large!\n");
  }
  memset(Buffer, 0, sizeof(Buffer));
  uint32_t idx = 0;
  for (int k=0; k<nchunks-1; k++) {
    memcpy(&Buffer[idx], chunks[k]->Buffer, chunks[k]->NBuffer);
    idx += chunks[k]->NBuffer;
  }
  idx += infosize;
  memcpy(&Buffer[idx], Data.Buffer, Data.NBuffer);
}
