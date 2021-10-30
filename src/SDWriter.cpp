#include <ContinuousADC.h>
#include <SDWriter.h>


SDCard::SDCard() {
  Available = false;
  NameCounter = 0;
  // start sdio interface to SD card:
  if (!SD.begin(SD_CONFIG))
    return;
  Available = true;
#ifdef SDCARD_USE_SDFAT
  SD.chvol();
#endif
}


SDCard::~SDCard() {
  end();
}


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
#endif
  NameCounter = 0;
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
      const char *fname = file.name();
      if (SD.remove(fname)) {
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
    } while (SD.exists(name.c_str()));
    return name;
  }
  else
    return fname;
}


SDWriter::SDWriter(const DataBuffer &data) :
  DataConsumer(&data) {
  SD = new SDCard;
  SDOwn = true;
  DataFile.close();
  WriteInterval = 100;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::SDWriter(SDCard &sd, const DataBuffer &data) :
  DataConsumer(&data) {
  SD = &sd;
  SDOwn = false;
  DataFile.close();
  WriteInterval = 100;
  FileSamples = 0;
  FileMaxSamples = 0;
}


SDWriter::~SDWriter() {
  end();
}


bool SDWriter::available() {
  return (SD != NULL && SD->available());
}


void SDWriter::end() {
  if (available()) {
    if (DataFile)
      DataFile.close();
    if (SDOwn) {
      SD->end();
      delete SD;
      SDOwn = false;
    }
    SD = NULL;
  }
}


void SDWriter::dataDir(const char *path) {
  if (available())
    SD->dataDir(path);
}


String SDWriter::incrementFileName(const String &fname) {
  if (available())
    return SD->incrementFileName(fname);
  else
    return "";
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
  if (! available() || strlen(fname) == 0)
    return false;
  if (DataFile) {
    Serial.println("failed to open file because a file is still open.");
    return false;
  }
  DataFile = SD->open(fname, O_WRITE | O_CREAT);
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


void SDWriter::openWave(const char *fname, const ContinuousADC &adc, int32_t samples,
	 	        char *datetime) {
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
  size_t head = Data->head();
  if (head == 0 || Tail == head)
    return 0;
  size_t nwrite = 0;
  if (Tail > head) {
    nwrite = Data->nbuffer() - Tail;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    if (nwrite > 0) {
      nbytes = DataFile.write((void *)&Data->buffer()[Tail], sizeof(sample_t)*nwrite);
      samples0 = nbytes / sizeof(sample_t);
      Tail += samples0;
      if (Tail >= Data->nbuffer()) {
	Tail -= Data->nbuffer();
	TailCycle++;
      }
      FileSamples += samples0;
    }
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = head - Tail;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  if (nwrite > 0) {
    nbytes = DataFile.write((void *)&Data->buffer()[Tail], sizeof(sample_t)*nwrite);
    samples1 = nbytes / sizeof(sample_t);
    Tail += samples1;
    if (Tail >= Data->nbuffer()) {
      Tail -= Data->nbuffer();
      TailCycle++;
    }
    FileSamples += samples1;
  }
  return samples0 + samples1;
}


void SDWriter::startWrite() {
  if (Data == NULL) {
    Serial.println("ERROR in SDWriter::startWrite(): Data buffer not initialized yet. ");
    Tail = 0;
    TailCycle = 0;
    return;
  }
  Tail = Data->head();
  TailCycle = Data->headCycle();
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
  DataConsumer::reset();
  FileSamples = 0;
}
