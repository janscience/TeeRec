#include <Arduino.h>
#include <SDWriter.h>

// TODO: What happens if there is no SD card?

SDWriter::SDWriter() {
  NameCounter = 0;
  // start sdio interface to SD card:
  if (!SD.begin(SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(50)))) {
    SDAvailable = false;
    SD.initErrorPrint();
    return;
  }    
  SD.chvol();
  SDAvailable = true;
}


SDWriter::~SDWriter() {
  end();
}


bool SDWriter::available() {
  return SDAvailable;
}


void SDWriter::end() {
  if ( SDAvailable )  // end sdio interface to SD card:
    SD.end();
}


void SDWriter::dataDir(const char *path) {
  if ( ! SDAvailable )
    return;
  if ( ! SD.exists(path) )
    SD.mkdir(path);
  SD.chdir(path);
  NameCounter = 0;
}


String SDWriter::incrementFileName(const String &fname) {
  if ( ! SDAvailable )
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
	if ( major > 25 ) {
	  Serial.println("file name overflow");
	  return "";
	}
	aa[0] = char('a' + major);
	name.replace("ANUM", aa);
      }
      else if (num) {
	if ( NameCounter > 99 ) {
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
  if ( ! SDAvailable )
    return;
  if (!File.open(fname, O_WRITE | O_CREAT)) {
    SD.errorHalt("failed to open file");
  }
}


void SDWriter::close() {
  if ( ! SDAvailable )
    return;
  if (!File.close())
    Serial.println("failed to close file");
}


FsFile &SDWriter::file() {
  return File;
}


void SDWriter::setupWaveHeader(uint8_t nchannels, uint32_t samplerate,
                               uint16_t resolution, int32_t samples) {
  NChannels = nchannels;
  NBytes = (resolution-1)/8  + 1;       // bytes per sample
  uint32_t datasize = samples * NBytes; // in bytes, nchannels is already in samples

  // RIFF chunk descriptor:
  char riff[] = "RIFF";
  strncpy(WaveHeader.mainChunkId, riff, 4);
  WaveHeader.mainChunkSize = 36 + datasize;
    // Size of the entire file -8 bytes for the two fields not included in this count (ChunkID and ChunkSize) 
  char wav[] = "WAVE";
  strncpy(WaveHeader.mainChunkFormat, wav, 4);
  
  // Subchunk1 --> fmt sub-chunk:
  char fmt[] = "fmt ";
  strncpy(WaveHeader.fmtChunkId, fmt, 4);
  WaveHeader.fmtChunkSize = 16; // 16 for PCM
  WaveHeader.formatTag = 1;     // 1 is PCM
  WaveHeader.numChannels = nchannels;
  WaveHeader.sampleRate = samplerate;
  WaveHeader.byteRate = samplerate * NBytes * NChannels;
  WaveHeader.blockAlign = NChannels * NBytes;
  WaveHeader.bitsPerSample = resolution;
  
  // Subchunk2 contains size of data and the actual data:
  char data[] = "data";
  strncpy(WaveHeader.SubtwoChunkId, data, 4);
  WaveHeader.SubtwoChunkSize = datasize;
  
  // This header makes a total of 44 bytes in size.
  // For more information visit http://soundfile.sapp.org/doc/WaveFormat/
}


void SDWriter::setupWaveHeader(const ContinuousADC &adcc, int32_t samples) {
  if ( samples < 0 )
    samples = adcc.maxFileSamples();
  setupWaveHeader(adcc.nchannels(), adcc.rate(), adcc.dataResolution(), samples);
}


void SDWriter::openWave(const char *fname) {
  if ( ! SDAvailable )
    return;
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  open(name.c_str());
  File.write((const uint8_t *)&WaveHeader, 44);
}


void SDWriter::closeWave(uint32_t samples) {
  if ( ! SDAvailable )
    return;
  if ( samples > 0 ) {
    uint32_t datasize = samples * NBytes;   // in bytes
    uint32_t mainchunksize = 36 + datasize;
    File.seek((char *)&WaveHeader.mainChunkSize - (char *)&WaveHeader);
    File.write((const uint8_t *)&mainchunksize, 4);
    File.seek((const uint8_t *)&WaveHeader.SubtwoChunkSize - (const uint8_t *)&WaveHeader);
    File.write((const uint8_t *)&datasize, 4);
  }
  close();
}
