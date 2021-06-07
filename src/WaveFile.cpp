#include <ContinuousADC.h>
#include <SDWriter.h>
#include <WaveFile.h>


WaveFile::Chunk::Chunk(const char *id, uint32_t size) {
  size = ((size+1) >> 1) << 1; // even size
  strncpy(Header.Id, id, 4);
  Header.Size = size;
  NBuffer = sizeof(Header) + Header.Size;
  Buffer = (char *)&this->Header;
}


void WaveFile::Chunk::addSize(uint32_t size) {
  Header.Size += size;
}


WaveFile::ListChunk::ListChunk(const char *id, const char *listid) :
  Chunk(id, sizeof(ListId)) {
  strncpy(ListId, listid, 4);
}


WaveFile::FormatChunk::FormatChunk(uint8_t nchannels, uint32_t samplerate,
				   uint16_t resolution) :
  Chunk("fmt ", sizeof(Format)) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Format.formatTag = 1;                   // 1 is PCM
  Format.numChannels = nchannels;
  Format.sampleRate = samplerate;
  Format.byteRate = samplerate * nchannels * nbytes;
  Format.blockAlign = nchannels * nbytes;
  Format.bitsPerSample = resolution;
}


WaveFile::InfoChunk::InfoChunk(const char *infoid, const char *text) :
  Chunk(infoid, strlen(text)) {
  memset(Text, 0, sizeof(Text));
  strcpy(Text, text);
}


WaveFile::DataChunk::DataChunk(uint16_t resolution, int32_t samples) :
  Chunk("data", 0) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Header.Size = samples * nbytes;         // in bytes, nchannels is already in samples
}


void WaveFile::writeHeader(FsFile &file, uint8_t nchannels, uint32_t samplerate,
			   uint16_t resolution, uint16_t dataresolution, int32_t samples,
			   char *datetimes) {
  // riff chunks:
  int nchunks = 0;
  Chunk *chunks[10];
  ListChunk riff("RIFF", "WAVE");
  chunks[nchunks++] = &riff;
  FormatChunk format(nchannels, samplerate, dataresolution);
  chunks[nchunks++] = &format;
  ListChunk info("LIST", "INFO");
  chunks[nchunks++] = &info;
  InfoChunk datetime("DTIM", datetimes==0?"":datetimes);
  if (datetimes != 0)
    chunks[nchunks++] = &datetime;
  InfoChunk software("ISFT", "TeeRec");
  chunks[nchunks++] = &software;
  char bs[4];
  sprintf(bs, "%d", resolution);
  InfoChunk bits("BITS", bs);
  chunks[nchunks++] = &bits;
  DataChunk data(dataresolution, samples);
  chunks[nchunks++] = &data;
  // update file size:
  riff.Header.Size = 4;
  for (int k=1; k<nchunks; k++)
    riff.Header.Size += chunks[k]->NBuffer;
  uint32_t nheader = 8 + riff.Header.Size;
  riff.Header.Size += data.Header.Size;
  // update info size:
  for (int k=3; k<nchunks-1; k++)
    info.addSize(chunks[k]->NBuffer);
  // make header:
  char header[nheader];
  uint32_t idx = 0;
  for (int k=0; k<nchunks; k++) {
    memcpy(&header[idx], chunks[k]->Buffer, chunks[k]->NBuffer);
    idx += chunks[k]->NBuffer;
  }
  file.write((const uint8_t *)&header, nheader);
}


void WaveFile::open(SDWriter &file, const char *fname,
		    const ContinuousADC &adc, int32_t samples, char *datetime) {
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  if (!file.open(name.c_str()))
    return;
  if (samples < 0)
    samples = adc.maxFileSamples();
  writeHeader(file.file(), adc.nchannels(), adc.rate(), adc.resolution(),
	      adc.dataResolution(), samples, datetime);
}


void WaveFile::close(SDWriter &file, const ContinuousADC &adc, uint32_t samples,
                     char *datetime) {
  if (! file.isOpen())
    return;
  if (samples > 0) {
    file.file().seek(0);
    writeHeader(file.file(), adc.nchannels(), adc.rate(), adc.resolution(),
		adc.dataResolution(), samples, datetime);
  }
  file.close();
}
