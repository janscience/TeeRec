#include <DataBuffer.h>
#include <SDWriter.h>


//#define DEBUG 1


SDCard::SDCard() {
  Available = false;
  NameCounter = 0;
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
  if (!SDClass::begin(csPin))
    return false;
  Available = true;
  sdfs.chvol();
  return true;
}


void SDCard::end() {
  if (Available)
    sdfs.end();
  Available = false;
}


bool SDCard::isBusy() {
  return sdfs.isBusy();
}


bool SDCard::dataDir(const char *path) {
  if (! Available)
    return false;
  if (! exists(path))
    mkdir(path);
  NameCounter = 0;
  return sdfs.chdir(path);
}


bool SDCard::rootDir() {
  if (! Available)
    return false;
  return sdfs.chdir("/");
}


void SDCard::listFiles(const char *path, Stream &stream) {
  SdFile file;
  if (! Available)
    return;
  SdFile dir;
  if (!dir.open(path)) {
    stream.printf("! ERROR: Folder \"%s\" does not exist.\n", path);
    return;
  }
  stream.printf("Files in \"%s\":\n", path);
  int n = 0;
  while (file.openNext(&dir, O_WRITE)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      stream.print("  ");
      stream.println(fname);
      n++;
    }
  }
  if (n > 0)
    stream.printf("%d files found.\n");
  else
    stream.printf("No files found.\n");
}


void SDCard::removeFiles(const char *path, Stream &stream) {
  SdFile file;
  if (! Available)
    return;
  SdFile dir;
  if (!dir.open(path)) {
    stream.printf("! ERROR: Folder \"%s\" does not exist.\n", path);
    return;
  }
  stream.printf("Removing all files in \"%s\":\n", path);
  int n = 0;
  while (file.openNext(&dir, O_WRITE)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      if (file.remove()) {
	stream.print("  ");
	stream.println(fname);
	n++;
      }
      else {
	stream.print("  Failed to remove file ");
	stream.println(fname);
      }
    }
  }
  if (n == 0)
    stream.println("No files found.");
  else
    stream.println("Done.");
}


void SDCard::cardType(char *types) {
  switch (sdfs.card()->type()) {
    case SD_CARD_TYPE_SD1:
      strcpy(types, "SD1");
      break;

    case SD_CARD_TYPE_SD2:
      strcpy(types, "SD2");
      break;

    case SD_CARD_TYPE_SDHC:
      csd_t m_csd;
      sdfs.card()->readCSD(&m_csd);
      if (sdCardCapacity(&m_csd) < 70000000)
	strcpy(types, "SDHC");
      else
	strcpy(types, "SDXC");
      break;

    default:
      strcpy(types, "unknown");
  }
}


float SDCard::capacity() {
  csd_t m_csd;
  if (!sdfs.card()->readCSD(&m_csd))
    return 0.0;
  return 512.0 * sdCardCapacity(&m_csd); // sectors a 512 byte
}


float SDCard::free() {
  return 512.0 * sdfs.freeClusterCount() * sdfs.sectorsPerCluster();
}


void SDCard::report(Stream &stream) {
  char types[10];
  cardType(types);

  char files[10];
  int ft = sdfs.fatType();
  if (ft > 32)
    strcpy(files, "exFAT");
  else
    sprintf(files, "FAT%d", ft);
  
  float cap = capacity();
  if (cap < 1.0) {
    stream.println("! ERROR: Failed to get sector count of SD card.");
    return;
  }
  
  stream.println("SD card:");
  stream.printf("  Type       : %s\n", types);
  stream.printf("  File system: %s\n", files);
  stream.printf("  Capacity   : %.3f GB\n", 1e-9 * cap);
  stream.printf("  Available  : %.3f GB\n", 1e-9 * free());
  stream.println();
}


void SDCard::erase(Stream &stream) {
  uint32_t const ERASE_SIZE = 262144L;
  uint32_t nsectors = 0;
  uint32_t first_block = 0;
  uint32_t last_block;
  uint16_t n = 0;
  
  stream.println("Erase SD card:");
  nsectors = sdfs.card()->sectorCount();
  do {
    last_block = first_block + ERASE_SIZE - 1;
    if (last_block >= nsectors) {
      last_block = nsectors - 1;
    }
    if (!sdfs.card()->erase(first_block, last_block)) {
      stream.println("erase failed");
      break;
    }
    stream.print('.');
    if ((n++)%64 == 63)
      stream.println();
    first_block += ERASE_SIZE;
  } while (first_block < nsectors);
  stream.println();
  stream.println("done.");
  stream.println();
}


void SDCard::format(const char *path, bool erase_card, Stream &stream) {
  File file;
  size_t n = 10;
  // read file:
  if (path != 0) {
    stream.printf("Read file \"%s\" ...\n", path);
    rootDir();
    file = openRead(path);
    n = file.available();
  }
  char buffer[n];
  if (path != 0) {
    file.read(buffer, n);
    file.close();
    stream.println();
  }
  // erase SD card:
  if (erase_card)
    erase();
  // format SD card:
  stream.println("Format SD card:");
  uint32_t nsectors = sdfs.card()->sectorCount();
  uint8_t sector_buffer[512];
  if (nsectors > 67108864) {   // larger than 32GB
    stream.println("Using ExFAT formatter");
    ExFatFormatter exfat_formatter;
    exfat_formatter.format(sdfs.card(), sector_buffer, &stream);
  }
  else {
    stream.println("Using FAT formatter");
    FatFormatter fat_formatter;
    fat_formatter.format(sdfs.card(), sector_buffer, &stream);
  }
  stream.println();
  // write file:
  if (path != 0) {
    file = openWrite(path);
    file.write(buffer, n);
    file.close();
    stream.printf("Restored file \"%s\".\n", path);
  }
}


String SDCard::incrementFileName(const String &fname) {
  if (! Available)
    return "";
  int numinx = -1;
  bool num = false;
  bool anum = (fname.indexOf("ANUM") >= 0);
  if (!anum) {
    numinx = fname.indexOf("NUM");
    num = (numinx >= 0);
  }
  int width = 2;
  char nums[6] = "NUM";
  if (num && numinx+4 < (int)fname.length() && isdigit(fname[numinx+3])) {
      width = fname[numinx+3] - '0';
      nums[3] = fname[numinx+3];
      nums[4] = '\0';
  }
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
	char nn[12];
	int maxn = 1;
	for (int w=0; w<width; w++)
	  maxn *= 10;
	maxn -= 1;
	if (NameCounter > maxn) {
	  Serial.println("WARNING: file name overflow");
	  return "";
	}
	volatile int nn_size = sizeof(nn); // avoid truncation warning: https://stackoverflow.com/a/70938456
	snprintf(nn, nn_size, "%0*d", width, NameCounter);
	name.replace(nums, nn);
      }
    } while (exists(name.c_str()));
    return name;
  }
  else
    return fname;
}

    
void SDCard::resetFileCounter() {
  NameCounter = 0;
}


File SDCard::openRead(const char *path) {
  return open(path, FILE_READ);
}


File SDCard::openWrite(const char *path) {
  return open(path, FILE_WRITE_BEGIN);
}


File SDCard::openAppend(const char *path) {
  return open(path, FILE_WRITE);
}


SDWriter::SDWriter(const DataWorker &producer) :
  DataWorker(&producer) {
  SDC = new SDCard;
  SDOwn = true;
  FileName = "";
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
  FileName = "";
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


float SDWriter::writeInterval() const {
  return 0.001*WriteInterval;
}

 
void SDWriter::setWriteInterval(float time) {
  if (time < 0)
    WriteInterval = uint(-1000*time*Data->bufferTime()); // fraction of the buffer
  else
    WriteInterval = uint(1000*time);                     // time interval in seconds
  if (0.001*WriteInterval > 0.5*Data->bufferTime())
    Serial.println("WARNING! SDWriter::setWriteInterval() interval larger than half the buffer!");
}


float SDWriter::writeTime() const {
  return 0.001*WriteTime;
}


bool SDWriter::pending() {
  //return (DataFile && available() >= 2048 && SDC != 0 && !SDC->isBusy()) {
  // or a combination of samples and writetime!!!
  return (DataFile && WriteTime > WriteInterval && SDC != 0 && !SDC->isBusy());
}


bool SDWriter::open(const char *fname) {
  if (! cardAvailable() || strlen(fname) == 0)
    return false;
  if (DataFile) {
    Serial.println("failed to open file because the file is still open.");
    return false;
  }
  FileName = fname;
  DataFile = SDC->openWrite(fname);
  FileSamples = 0;
  return isOpen();
}


bool SDWriter::isOpen() const {
  return bool(DataFile);
}


void SDWriter::close() {
  DataFile.close();
}


bool SDWriter::openWave(const char *fname, int32_t samples,
			const char *datetime) {
  if (!open(fname))
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
  return (DataFile) ? true : false;
}


bool SDWriter::closeWave() {
  if (! (DataFile))
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
  close();
  return success;
}


String SDWriter::baseName() const {
  int idx = FileName.lastIndexOf('.');
  if (idx >= 0)
    return FileName.substring(0, idx);
  else
    return FileName;
}


ssize_t SDWriter::write() {
  size_t nbytes = 0;
  size_t samples0 = 0;
  size_t samples1 = 0;
  if (! (DataFile))
    return -1;
  if (FileMaxSamples > 0 && FileSamples >= FileMaxSamples)
    return -2;
  size_t missed = overrun();
  if (missed > 0) {
#ifdef DEBUG
    Serial.printf("ERROR in SDWriter::write(): Data overrun! Missed %d samples (%.0f%% of buffer, %.0fms).\n", missed, 100.0*missed/Data->nbuffer(), 1000*Data->time(missed));
    int wt = WriteTime;
    Serial.printf("----> last write %dms ago.\n", wt);
#endif
    return -4;
  }
  if (available() == 0) {
    if (writeTime() > 4*Data->DMABufferTime()) {
#ifdef DEBUG
      Serial.printf(" SD cycle: %5d,  SD index: %6d\n", Cycle, Index);
      Serial.printf("TDM cycle: %5d, TDM index: %6d\n", Data->cycle(), Data->index());
#endif
      return -3;
    }
    else
      return 0;
  }
#ifdef DEBUG
  if (WriteTime > 30) {
    int wt = WriteTime;
    Serial.printf("----> last write %dms ago.\n", wt);
  }
#endif
  size_t index = Producer->index();
  size_t nwrite = 0;
  if (Index >= index) {
    nwrite = Data->nbuffer() - Index;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    if (nwrite > 0) {
      nbytes = DataFile.write((void *)&Data->buffer()[Index], sizeof(sample_t)*nwrite);
      if (nbytes == 0)
	return -5;
#ifdef DEBUG
      if (WriteTime > 30) {
	int wt = WriteTime;
	Serial.printf("----> needed %dms for writing end-of-buffer data to SD card.\n", wt);
      }
#endif
      WriteTime = 0;
      samples0 = nbytes / sizeof(sample_t);
      increment(samples0);
      FileSamples += samples0;
      if (samples0 < nwrite) {
	Serial.printf("WARNING: only wrote %d samples of %d to the end of the data buffer\n", samples0, nwrite);
	return samples0;
      }
      if (FileMaxSamples > 0 && FileSamples >= FileMaxSamples)
	return samples0;
      index = Producer->index();
    }
  }
  nwrite = index - Index;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  nwrite = (nwrite/MajorSize)*MajorSize;          // write only full blocks
  if (nwrite > 0) {
    nbytes = DataFile.write((void *)&Data->buffer()[Index], sizeof(sample_t)*nwrite);
    if (nbytes == 0)
      return -5;
    WriteTime = 0;
    samples1 = nbytes / sizeof(sample_t);
    if (samples1 < nwrite)
      Serial.printf("WARNING: only wrote %d samples of %d\n", samples1, nwrite);
    increment(samples1);
    FileSamples += samples1;
  }
#ifdef DEBUG
  if (WriteTime > 30) {
    int wt = WriteTime;
    Serial.printf("----> needed %dms for writing beginning-of-buffer data to SD card.\n", wt);
  }
#endif
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
  if (Data->rate() == 0)
    Serial.println("WARNING in SDWriter::setMaxFileTime(): sampling rate not yet set!");
  setMaxFileSamples(Data->samples(secs));
}


size_t SDWriter::maxFileSamples() const {
  return FileMaxSamples;
}


float SDWriter::maxFileTime() const {
  return Data->time(FileMaxSamples);
}


bool SDWriter::endWrite() {
  return (FileMaxSamples > 0 && FileSamples >= FileMaxSamples);
}


void SDWriter::reset() {
  DataWorker::reset();
  FileSamples = 0;
}
