#include <ContinuousADC.h>
#include <SdFat.h>
#include <WaveFile.h>


void WaveFile::writeHeader(FsFile &file, uint8_t nchannels, uint32_t samplerate,
			   uint16_t resolution, int32_t samples) {
  size_t nbytes = (resolution-1)/8  + 1; // bytes per sample
  uint32_t datasize = samples * nbytes;  // in bytes, nchannels is already in samples

  WaveHeader header;
  // RIFF chunk descriptor:
  strncpy(header.mainChunkId, "RIFF", 4);
  header.mainChunkSize = 36 + datasize;
    // Size of the entire file -8 bytes for the two fields not included in this count (ChunkID and ChunkSize) 
  strncpy(header.mainChunkFormat, "WAVE", 4);
  
  // Subchunk1 --> fmt sub-chunk:
  strncpy(header.fmtChunkId, "fmt ", 4);
  header.fmtChunkSize = 16; // 16 for PCM
  header.formatTag = 1;     // 1 is PCM
  header.numChannels = nchannels;
  header.sampleRate = samplerate;
  header.byteRate = samplerate * nchannels * nbytes;
  header.blockAlign = nchannels * nbytes;
  header.bitsPerSample = resolution;
  
  // Subchunk2 contains size of data and the actual data:
  strncpy(header.SubtwoChunkId, "data", 4);
  header.SubtwoChunkSize = datasize;
  
  // This header makes a total of 44 bytes in size.
  // For more information visit http://soundfile.sapp.org/doc/WaveFormat/
  file.write((const uint8_t *)&header, 44);
}


void WaveFile::open(FsFile &file, const char *fname,
		    const ContinuousADC &adc, int32_t samples) {
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  if (!file.open(name.c_str()))
    return;
  if (samples < 0)
    samples = adc.maxFileSamples();
  writeHeader(file, adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
}


void WaveFile::close(FsFile &file, const ContinuousADC &adc, uint32_t samples) {
  if (! file.isOpen())
    return;
  if (samples > 0) {
    file.seek(0);
    writeHeader(file, adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
  }
  file.close();
}
