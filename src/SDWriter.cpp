#include <ContinuousADC.h>
#include <SDWriter.h>


SDWriter::SDWriter() :
  DataConsumer() {
  NameCounter = 0;
  // start sdio interface to SD card:
  if (!SD.begin(SD_CONFIG)) {
    SDAvailable = false;
    return;
  }    
  SD.chvol();
  SDAvailable = true;
  File.close();
  WriteInterval = 100;
  FileSamples = 0;
  FileMaxSamples = 0;
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


void SDWriter::removeFiles(const char *path) {
  FsFile dir;
  FsFile file;
  if (! SDAvailable)
    return;
  if (!dir.open(path))
    return;
  Serial.printf("Removing all files in %s:\n", path);
  while (file.openNext(&dir, O_WRITE)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      if (file.remove()) {
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


bool SDWriter::open(const char *fname) {
  if (! SDAvailable || strlen(fname) == 0)
    return false;
  if (File.isOpen()) {
    Serial.println("failed to open file because a file is still open.");
    return false;
  }
  if (!File.open(fname, O_WRITE | O_CREAT))
    Serial.printf("failed to open file %s\n", fname);
  FileSamples = 0;
  WriteTime = 0;
  return File.isOpen();
}


bool SDWriter::isOpen() const {
  return File.isOpen();
}


void SDWriter::close() {
  if (! File.isOpen())
    return;
  if (!File.close())
    Serial.println("failed to close file");
}


FsFile &SDWriter::file() {
  return File;
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
  if (File.write(Wave.Buffer, Wave.NBuffer) < 0) {  // 14ms
    Serial.println("ERROR writing wave header");
  }
}


void SDWriter::closeWave() {
  if (! File.isOpen())
    return;
  if (FileSamples > 0) {
    Wave.setData(FileSamples);
    Wave.assemble();
    File.seek(0);
    File.write(Wave.Buffer, Wave.NBuffer);   // 2ms
  }
  close();                                   // 6ms
}


size_t SDWriter::writeData() {
  size_t nbytes = 0;
  size_t samples0 = 0;
  size_t samples1 = 0;
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return 0;
  if (! File.isOpen())
    return 0;
  size_t head = Data->head();
  size_t nwrite = 0;
  if (Tail >= head && ! (head == 0 && Tail ==0)) {
    nwrite = Data->NBuffer - Tail;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    if (nwrite > 0) {
      nbytes = File.write((void *)&Data->Buffer[Tail], sizeof(sample_t)*nwrite);
      samples0 = nbytes / sizeof(sample_t);
      Tail += samples0;
      if (Tail >= Data->NBuffer)
	Tail -= Data->NBuffer;
      FileSamples += samples0;
    }
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = head - Tail;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  if (nwrite > 0) {
    nbytes = File.write((void *)&Data->Buffer[Tail], sizeof(sample_t)*nwrite);
    samples1 = nbytes / sizeof(sample_t);
    Tail += samples1;
    if (Tail >= Data->NBuffer)
      Tail -= Data->NBuffer;
    FileSamples += samples1;
  }
  return samples0 + samples1;
}


void SDWriter::startWrite() {
  Tail = Data->head();
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
