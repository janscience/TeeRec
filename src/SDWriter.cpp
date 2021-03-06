#include <DataBuffer.h>
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


#ifdef BUILTIN_SDCARD
bool SDCard::begin() {
  return begin(BUILTIN_SDCARD);
}
#endif


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


bool SDCard::isBusy() {
#ifdef SDCARD_USE_SDFAT
  return SD.isBusy();
#else
  return false;
#endif
}


bool SDCard::dataDir(const char *path) {
  if (! Available)
    return false;
  if (! SD.exists(path))
    SD.mkdir(path);
  NameCounter = 0;
#ifdef SDCARD_USE_SDFAT
  return SD.chdir(path);
#else
  CurrentPath = path;
  CurrentPath += "/";
  return true;
#endif
}


bool SDCard::rootDir() {
  if (! Available)
    return false;
#ifdef SDCARD_USE_SDFAT
  return SD.chdir("/");
#else
  CurrentPath = "/";
  return true;
#endif
}


bool SDCard::exists(const char *path) {
#ifdef SDCARD_USE_SDFAT
  return SD.exists(path);
#else
  return SD.exists((CurrentPath + name).c_str());
#endif
}


bool SDCard::removeFile(const char *path) {
  FsFile file;
#ifdef SDCARD_USE_SDFAT
  return SD.remove(path);
#else
  return SD.remove((CurrentPath + path).c_str());
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


FsFile SDCard::openAppend(const char *path) {
#ifdef SDCARD_USE_SDFAT
  return SD.open(path, O_RDWR | O_APPEND);
#else
  return SD.open((CurrentPath + path).c_str(), FILE_WRITE);
#endif
}


SDWriter::SDWriter(const DataWorker &producer) :
  DataWorker(&producer) {
  SDC = new SDCard;
  SDOwn = true;
  DataFile.close();
  WriteInterval = 100;
  WriteTime = 0;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::SDWriter(SDCard &sd, const DataWorker &producer) :
  DataWorker(&producer) {
  SDC = &sd;
  SDOwn = false;
  DataFile.close();
  WriteInterval = 100;
  WriteTime = 0;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::~SDWriter() {
  end();
}


bool SDWriter::cardAvailable() const {
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
  WriteTime = 0;
  FileSamples = 0;
}


bool SDWriter::dataDir(const char *path) {
  return SDC->dataDir(path);
}


bool SDWriter::rootDir() {
  return SDC->rootDir();
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


float SDWriter::writeInterval() const {
  return 0.001*WriteInterval;
}

 
void SDWriter::setWriteInterval() {
  WriteInterval = uint(250*Data->bufferTime()); // a quarter of the buffer
  //WriteInterval = uint(100*Data->bufferTime()); // a tenth of the buffer
}


float SDWriter::writeTime() const {
  return 0.001*WriteTime;
}


bool SDWriter::pending() {
  //if (DataFile && available() >= 2048 && SDC != 0 && !SDC->isBusy()) {
  // or a combination of samples and writetime!!!
  if (DataFile && WriteTime > WriteInterval && SDC != 0 && !SDC->isBusy()) {
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
    Serial.println("failed to open file because the file is still open.");
    return false;
  }
  DataFile = SDC->openWrite(fname);
  FileSamples = 0;
  WriteTime = 0;
  return DataFile ? true : false;
}


bool SDWriter::isOpen() const {
  return DataFile ? true : false;
}


bool SDWriter::close() {
  if (! DataFile)
    return true;
  return DataFile.close();
}


FsFile &SDWriter::file() {
  return DataFile;
}


bool SDWriter::openWave(const char *fname, int32_t samples,
			const char *datetime) {
  String name(fname);
  if (name.indexOf('.') < 0 )
    name += ".wav";
  if (!open(name.c_str()))                // 11ms
    return false;
  if (samples < 0)
    samples = FileMaxSamples;
  Wave.setFormat(Data->nchannels(), Data->rate(), Data->resolution(),
		 Data->dataResolution());
  setWaveHeader(Wave);    // recursively calls setWaveHeader on all producers.
  Wave.setData(samples);
  if (datetime != 0)
    Wave.setDateTime(datetime);
  else
    Wave.clearDateTime();
  Wave.assemble();                        // 0ms
  if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {  // 14ms
    Serial.println("ERROR: initial writing of wave header");
    return false;
  }
  return DataFile ? true : false;
}


bool SDWriter::closeWave() {
  if (! DataFile)
    return true;
  bool success = true;
  if (FileSamples > 0) {
    Wave.setData(FileSamples);
    Wave.assemble();
    DataFile.seek(0);
    if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {  // 2ms
      Serial.println("ERROR: final writing of wave header");
      success = false;
    }
  }
  if (! close())
    success = false;
  return success;
}


ssize_t SDWriter::write() {
  size_t nbytes = 0;
  size_t samples0 = 0;
  size_t samples1 = 0;
  if (! DataFile)
    return -1;
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return -2;
  size_t missed = overrun();
  if (missed > 0)
    Serial.printf("ERROR in SDWriter::writeData(): Data overrun! Missed %d samples.\n", missed);
  if (available() == 0)
    return -3;
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
      if (samples0 < nwrite) {
	Serial.printf("ERROR: only wrote %d samples of %d to the end of the data buffer\n", samples0, nwrite);
	return samples0;
      }
    }
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = index - Index;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  nwrite = (nwrite/MajorSize)*MajorSize;          // write only full blocks
  if (nwrite > 0) {
    nbytes = DataFile.write((void *)&Data->buffer()[Index], sizeof(sample_t)*nwrite);
    samples1 = nbytes / sizeof(sample_t);
    if (samples1 < nwrite)
      Serial.printf("ERROR: only wrote %d samples of %d\n", samples1, nwrite);
    increment(samples1);
    FileSamples += samples1;
  }
  return samples0 + samples1;
}


void SDWriter::start(size_t decr) {
  if (!synchronize())
    Serial.println("ERROR in SDWriter::startWrite(): Data buffer not initialized yet. ");
  WriteTime = 0;
  if (decr > 0) {
    decrement(decr);
    WriteTime += int(1000.0*Data->time(decr));
  }
}


size_t SDWriter::fileSamples() const {
  return FileSamples;
}


float SDWriter::fileTime() const {
  return Data->time(FileSamples);
}


void SDWriter::fileTimeStr(char *str) const {
  Data->timeStr(FileSamples, str);
}


void SDWriter::setMaxFileSamples(size_t samples) {
  FileMaxSamples = (samples/MajorSize)*MajorSize;
}


void SDWriter::setMaxFileTime(float secs) {
  setMaxFileSamples(Data->samples(secs));
}


size_t SDWriter::maxFileSamples() const {
  return FileMaxSamples;
}


float SDWriter::maxFileTime() const {
  return Data->time(FileMaxSamples);
}


bool SDWriter::endWrite() {
  return ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples );
}


void SDWriter::reset() {
  DataWorker::reset();
  FileSamples = 0;
}
