#include <ContinuousADC.h>
#include <SDWriter.h>
#include <WaveFile.h>


WaveFile::ChunkBuffer::ChunkBuffer() {
  Header = 0;
  Data = 0;
  Buffer = 0;
  NBuffer = 0;
}


WaveFile::ChunkBuffer::ChunkBuffer(const char *id, uint32_t size) {
  size = ((size+1) >> 1) << 1; // even size
  NBuffer = sizeof(Chunk) + size;
  Buffer = new char[NBuffer];
  memset(Buffer, 0, NBuffer);
  Header = (Chunk *)Buffer;
  strncpy(Header->Id, id, 4);
  Header->Size = size;
  Data = &Buffer[sizeof(Chunk)];
}


WaveFile::ChunkBuffer::~ChunkBuffer() {
  delete [] Buffer;
}


void WaveFile::ChunkBuffer::addSize(uint32_t size) {
  Header->Size += size;
}


WaveFile::RiffChunk::RiffChunk() :
  ChunkBuffer("RIFF", 4) {
  strncpy(Data, "WAVE", 4);
}


WaveFile::FormatChunk::FormatChunk(uint8_t nchannels, uint32_t samplerate,
				   uint16_t resolution) :
  ChunkBuffer("fmt ", sizeof(Format_t)) {
  Format = (Format_t *)Data;
  size_t nbytes = (resolution-1)/8  + 1;    // bytes per sample
  Format->formatTag = 1;                    // 1 is PCM
  Format->numChannels = nchannels;
  Format->sampleRate = samplerate;
  Format->byteRate = samplerate * nchannels * nbytes;
  Format->blockAlign = nchannels * nbytes;
  Format->bitsPerSample = resolution;
}


WaveFile::DataChunk::DataChunk(uint16_t resolution, int32_t samples) :
  ChunkBuffer("data", 0) {
  size_t nbytes = (resolution-1)/8  + 1;  // bytes per sample
  Header->Size = samples * nbytes;        // in bytes, nchannels is already in samples
}


void WaveFile::writeHeader(FsFile &file, uint8_t nchannels, uint32_t samplerate,
			   uint16_t resolution, int32_t samples) {
  RiffChunk riff;
  FormatChunk format(nchannels, samplerate, resolution);
  DataChunk data(resolution, samples);
  ChunkBuffer *chunks[3] = {&riff, &format, &data};
  // update file size:
  riff.Header->Size = 4;
  for (int k=1; k<3; k++)
    riff.Header->Size += chunks[k]->NBuffer;
  uint32_t nheader = 8 + riff.Header->Size;
  riff.Header->Size += data.Header->Size;
  // make header:
  char header[nheader];
  uint32_t idx = 0;
  for (int k=0; k<3; k++) {
    memcpy(&header[idx], chunks[k]->Buffer, chunks[k]->NBuffer);
    idx += chunks[k]->NBuffer;
  }
  file.write((const uint8_t *)&header, nheader);
}


void WaveFile::open(SDWriter &file, const char *fname,
		    const ContinuousADC &adc, int32_t samples) {
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  if (!file.open(name.c_str()))
    return;
  if (samples < 0)
    samples = adc.maxFileSamples();
  writeHeader(file.file(), adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
}


void WaveFile::close(SDWriter &file, const ContinuousADC &adc, uint32_t samples) {
  if (! file.isOpen())
    return;
  if (samples > 0) {
    file.file().seek(0);
    writeHeader(file.file(), adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
  }
  file.close();
}
