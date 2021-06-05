#include <Arduino.h>
#include <ContinuousADC.h>
#include <SDWriter.h>


SDWriter::SDWriter() {
  NameCounter = 0;
  // start sdio interface to SD card:
  if (!SD.begin(SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(50)))) {
    SDAvailable = false;
    return;
  }    
  SD.chvol();
  SDAvailable = true;
  File.close();
  WriteInterval = 100;
}


SDWriter::~SDWriter() {
  end();
}


bool SDWriter::available() {
  return SDAvailable;
}


void SDWriter::end() {
  if (SDAvailable) {  // end sdio interface to SD card:
    if (File.isOpen())
      File.close();
    SD.end();
  }
}


void SDWriter::dataDir(const char *path) {
  if (! SDAvailable)
    return;
  if (! SD.exists(path))
    SD.mkdir(path);
  SD.chdir(path);
  NameCounter = 0;
}


void SDWriter::setWriteInterval(const ContinuousADC &adc) {
  WriteInterval = uint(250*adc.bufferTime()); // a quarter of the buffer
}


bool SDWriter::needToWrite() {
  if (File.isOpen() && WriteTime > WriteInterval) {
    WriteTime -= WriteInterval;
    return true;
  }
  else
    return false;
}


String SDWriter::incrementFileName(const String &fname) {
  if (! SDAvailable)
    return "";
  bool num = false;
  bool anum = (fname.indexOf("ANUM") >= 0);
  if (!anum)
    num = (fname.indexOf("NUM") >= 0);
  String aa("aa");
  if (num || anum) {
    String name;
    do {
      name = fname;
      NameCounter++;
      if (anum) {
	aa[1] = char('a' + ((NameCounter-1) % 26));
	uint16_t major = (NameCounter-1) / 26;
	if (major > 25) {
	  Serial.println("file name overflow");
	  return "";
	}
	aa[0] = char('a' + major);
	name.replace("ANUM", aa);
      }
      else if (num) {
	if (NameCounter > 99) {
	  Serial.println("file name overflow");
	  return "";
	}
	char nn[4];
	sprintf(nn, "%02d", NameCounter);
	name.replace("NUM", nn);
      }
    } while (SD.exists(name.c_str()));
    return name;
  }
  else
    return fname;
}


void SDWriter::open(const char *fname) {
  if (! SDAvailable || strlen(fname) == 0)
    return;
  if (File.isOpen()) {
    Serial.println("failed to open file because a file is still open.");
    return;
  }
  if (!File.open(fname, O_WRITE | O_CREAT))
    Serial.printf("failed to open file %s\n", fname);
  WriteTime = 0;
}


bool SDWriter::isOpen() const {
  return File.isOpen();
}


void SDWriter::close() {
  if (! SDAvailable || ! File.isOpen())
    return;
  if (!File.close())
    Serial.println("failed to close file");
}


FsFile &SDWriter::file() {
  return File;
}


void SDWriter::writeWaveHeader(uint8_t nchannels, uint32_t samplerate,
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
  File.write((const uint8_t *)&header, 44);
}


void SDWriter::openWave(const char *fname, const ContinuousADC &adc, int32_t samples) {
  if (! SDAvailable)
    return;
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  open(name.c_str());
  if (samples < 0)
    samples = adc.maxFileSamples();
  writeWaveHeader(adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
}


void SDWriter::closeWave(const ContinuousADC &adc, uint32_t samples) {
  if (! SDAvailable || ! File.isOpen())
    return;
  if (samples > 0) {
    File.seek(0);
    writeWaveHeader(adc.nchannels(), adc.rate(), adc.dataResolution(), samples);
  }
  close();
}
