#include <SDWriter.h>


SDCard::SDCard() {
  Available = false;
  NameCounter = 0;
#ifndef SDCARD_USE_SDFAT
  CurrentPath = "/";
#endif
}


SDCard::~SDCard() {
  end();
}


bool SDCard::begin() {
  return begin(BUILTIN_SDCARD);
}


bool SDCard::begin(uint8_t csPin) {
  if (!SD.begin(csPin))
    return false;
  Available = true;
#ifdef SDCARD_USE_SDFAT
  SD.chvol();
#else
  CurrentPath = "/";
#endif
  return true;
}


#ifdef SDCARD_USE_SDFAT

bool SDCard::begin(SdCsPin_t csPin, uint32_t maxSck) {
  if (!SD.begin(csPin, maxSck))
    return false;
  Available = true;
  SD.chvol();
  return true;
}


bool SDCard::begin(SdioConfig sdioConfig) {
  if (!SD.begin(sdioConfig))
    return false;
  Available = true;
  SD.chvol();
  return true;
}


bool SDCard::begin(SdSpiConfig spiConfig) {
  if (!SD.begin(spiConfig))
    return false;
  Available = true;
  SD.chvol();
  return true;
}

#endif


void SDCard::end() {
#ifdef SDCARD_USE_SDFAT
  if (Available)
    SD.end();
#endif
}


void SDCard::dataDir(const char *path) {
  if (! Available)
    return;
  if (! SD.exists(path))
    SD.mkdir(path);
#ifdef SDCARD_USE_SDFAT
  SD.chdir(path);
#else
  CurrentPath = path;
  CurrentPath += "/";
#endif
  NameCounter = 0;
}


void SDCard::rootDir() {
#ifdef SDCARD_USE_SDFAT
  SD.chdir("/");
#else
  CurrentPath = "/";
#endif
}


void SDCard::removeFiles(const char *path) {
  FsFile file;
  if (! Available)
    return;
#ifdef SDCARD_USE_SDFAT
  FsFile dir;
  if (!dir.open(path))
    return;
#else
  FsFile dir = SD.open(path);
  if (!dir)
    return;
#endif
  Serial.printf("Removing all files in %s:\n", path);
#ifdef SDCARD_USE_SDFAT
  while (file.openNext(&dir, O_WRITE)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      if (file.remove()) {
#else
  while (file = dir.openNextFile()) {
    if (!file.isDirectory()) {
      char file_path[200];
      strcpy(file_path, path);
      strcat(file_path, "/");
      strcat(file_path, file.name());
      const char *fname = file.name();
      if (SD.remove(file_path)) {
#endif
	Serial.print("  ");
	Serial.println(fname);
      }
      else {
	Serial.print("  Failed to remove file ");
	Serial.println(fname);
      }
    }
  }
  Serial.println("done");
}


String SDCard::incrementFileName(const String &fname) {
  if (! Available)
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
	  Serial.println("WARNING: file name overflow");
	  return "";
	}
	aa[0] = char('a' + major);
	name.replace("ANUM", aa);
      }
      else if (num) {
	if (NameCounter > 99) {
	  Serial.println("WARNING: file name overflow");
	  return "";
	}
	char nn[4];
	sprintf(nn, "%02d", NameCounter);
	name.replace("NUM", nn);
      }
#ifdef SDCARD_USE_SDFAT
    } while (SD.exists(name.c_str()));
#else
    } while (SD.exists((CurrentPath + name).c_str()));
#endif
    return name;
  }
  else
    return fname;
}

    
void SDCard::resetFileCounter() {
  NameCounter = 0;
}


FsFile SDCard::openRead(const char *path) {
#ifdef SDCARD_USE_SDFAT
  return SD.open(path, O_READ);
#else
  return SD.open((CurrentPath + path).c_str(), FILE_READ);
#endif
}

    
FsFile SDCard::openWrite(const char *path) {
#ifdef SDCARD_USE_SDFAT
  return SD.open(path, O_RDWR | O_CREAT);
#else
  return SD.open((CurrentPath + path).c_str(), FILE_WRITE_BEGIN);
#endif
}


SDWriter::SDWriter(const DataWorker &producer) :
  DataWorker(&producer) {
  SDC = new SDCard;
  SDOwn = true;
  DataFile.close();
  WriteInterval = 100;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::SDWriter(SDCard &sd, const DataWorker &producer) :
  DataWorker(&producer) {
  SDC = &sd;
  SDOwn = false;
  DataFile.close();
  WriteInterval = 100;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::~SDWriter() {
  end();
}


bool SDWriter::cardAvailable() {
  return (SDC != NULL && SDC->available());
}


void SDWriter::end() {
  if (cardAvailable()) {
    if (DataFile)
      DataFile.close();
    if (SDOwn) {
      SDC->end();
      delete SDC;
      SDOwn = false;
    }
    SDC = NULL;
  }
}


void SDWriter::dataDir(const char *path) {
  if (cardAvailable())
    SDC->dataDir(path);
}


void SDWriter::rootDir() {
  if (cardAvailable())
    SDC->rootDir();
}


String SDWriter::incrementFileName(const String &fname) {
  if (cardAvailable())
    return SDC->incrementFileName(fname);
  else
    return "";
}


void SDWriter::resetFileCounter() {
  if (cardAvailable())
    return SDC->resetFileCounter();
}


void SDWriter::setSoftware(const char *software) {
  Wave.setSoftware(software);
}

 
void SDWriter::setWriteInterval(const ContinuousADC &adc) {
  WriteInterval = uint(250*adc.bufferTime()); // a quarter of the buffer
}


bool SDWriter::needToWrite() {
  if (DataFile && WriteTime > WriteInterval) {
    WriteTime -= WriteInterval;
    return true;
  }
  else
    return false;
}


bool SDWriter::open(const char *fname) {
  if (! cardAvailable() || strlen(fname) == 0)
    return false;
  if (DataFile) {
    Serial.println("failed to open file because a file is still open.");
    return false;
  }
  DataFile = SDC->openWrite(fname);
  //if (DataFile.getError())
  if (!DataFile)
    Serial.printf("WARNING: failed to open file %s\n", fname);
  FileSamples = 0;
  WriteTime = 0;
  return DataFile ? true : false;
}


bool SDWriter::isOpen() const {
  return DataFile ? true : false;
}


void SDWriter::close() {
  if (! DataFile)
    return;
  DataFile.close();
}


FsFile &SDWriter::file() {
  return DataFile;
}


void SDWriter::openWave(const char *fname, const ContinuousADC &adc,
			int32_t samples, const char *datetime) {
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  if (!open(name.c_str()))                // 11ms
    return;
  if (samples < 0)
    samples = FileMaxSamples;
  Wave.setFormat(adc.nchannels(), adc.rate(), adc.resolution(),
		 adc.dataResolution());
  char channels[100];
  adc.channels(channels);
  Wave.setChannels(channels);
  Wave.setAveraging(adc.averaging());
  Wave.setConversionSpeed(adc.conversionSpeedShortStr());
  Wave.setSamplingSpeed(adc.samplingSpeedShortStr());
  Wave.setReference(adc.referenceStr());
  Wave.setData(samples);
  if (datetime != 0)
    Wave.setDateTime(datetime);
  else
    Wave.clearDateTime();
  Wave.assemble();                        // 0ms
  if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {  // 14ms
    Serial.println("ERROR writing wave header");
  }
}


void SDWriter::closeWave() {
  if (! DataFile)
    return;
  if (FileSamples > 0) {
    Wave.setData(FileSamples);
    Wave.assemble();
    DataFile.seek(0);
    if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {  // 2ms
      Serial.println("ERROR writing wave header");
    }
  }
  close();                                   // 6ms
}


size_t SDWriter::writeData() {
  size_t nbytes = 0;
  size_t samples0 = 0;
  size_t samples1 = 0;
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return 0;
  if (! DataFile)
    return 0;
  size_t missed = overrun();
  if (missed > 0)
    Serial.printf("ERROR in SDWriter::writeData(): Data overrun! Missed %d samples.\n", missed);
  if (available() == 0)
    return 0;
  size_t index = Producer->index();
  size_t nwrite = 0;
  if (Index >= index) {
    nwrite = Data->nbuffer() - Index;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    if (nwrite > 0) {
      nbytes = DataFile.write((void *)&Data->buffer()[Index], sizeof(sample_t)*nwrite);
      samples0 = nbytes / sizeof(sample_t);
      increment(samples0);
      FileSamples += samples0;
    }
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = index - Index;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  if (nwrite > 0) {
    nbytes = DataFile.write((void *)&Data->buffer()[Index], sizeof(sample_t)*nwrite);
    samples1 = nbytes / sizeof(sample_t);
    increment(samples1);
    FileSamples += samples1;
  }
  return samples0 + samples1;
}


void SDWriter::startWrite(size_t decr) {
  if (!synchronize())
    Serial.println("ERROR in SDWriter::startWrite(): Data buffer not initialized yet. ");
  if (decr > 0)
    decrement(decr);
}


size_t SDWriter::fileSamples() const {
  return FileSamples;
}


float SDWriter::fileTime() const {
  return ContinuousADC::ADCC->time(FileSamples);
}


void SDWriter::fileTimeStr(char *str) const {
  ContinuousADC::ADCC->timeStr(FileSamples, str);
}


void SDWriter::setMaxFileSamples(size_t samples) {
  FileMaxSamples = (samples/ContinuousADC::MajorSize)*ContinuousADC::MajorSize;
}


void SDWriter::setMaxFileTime(float secs) {
  setMaxFileSamples(ContinuousADC::ADCC->samples(secs));
}


size_t SDWriter::maxFileSamples() const {
  return FileMaxSamples;
}


bool SDWriter::endWrite() {
  return ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples );
}


void SDWriter::reset() {
  DataWorker::reset();
  FileSamples = 0;
}
