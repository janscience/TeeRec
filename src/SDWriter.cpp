#include <DataBuffer.h>
#include <SDWriter.h>


SDWriter::SDWriter(const DataWorker &producer, int verbose) :
  DataWorker(&producer, verbose),
  SDC(0),
  SDOwn(true),
  FileName(""),
  WriteTime(0),
  MaxWriteTime(100),
  WriteInterval(100),
  FileSamples(0),
  FileMaxSamples(0) {
  SDC = new SDCard;
  DataFile.close();
}


SDWriter::SDWriter(SDCard &sd, const DataWorker &producer, int verbose) :
  DataWorker(&producer, verbose),
  SDC(&sd),
  SDOwn(false),
  FileName(""),
  WriteTime(0),
  MaxWriteTime(100),
  WriteInterval(100),
  FileSamples(0),
  FileMaxSamples(0) {
  DataFile.close();
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
  elapsedMillis t = 0;
  if (! cardAvailable() || strlen(fname) == 0)
    return false;
  if (DataFile) {
    Serial.println("failed to open file because the file is still open.");
    return false;
  }
  FileName = fname;
  DataFile = SDC->openWrite(fname);
  FileSamples = 0;
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = t;
    Serial.printf("----> SDWriter::open() took %dms.\n", wt);
  }
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
  elapsedMillis t = 0;
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
  Wave.assemble();
  if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {
    Serial.println("ERROR: initial writing of wave header");
    return false;
  }
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = t;
    Serial.printf("----> SDWriter::openWave() took %dms.\n", wt);
  }
  return (DataFile) ? true : false;
}


bool SDWriter::openWave(const char *fname, const WaveHeader &wave) {
  elapsedMillis t = 0;
  if (!open(fname))
    return false;
  if (DataFile.write(wave.Buffer, wave.NBuffer) != wave.NBuffer) {
    Serial.println("ERROR: initial writing of wave header");
    return false;
  }
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = t;
    Serial.printf("----> SDWriter::openWave() took %dms.\n", wt);
  }
  return (DataFile) ? true : false;
}


bool SDWriter::closeWave() {
  if (! (DataFile))
    return true;
  elapsedMillis t = 0;
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
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = t;
    Serial.printf("----> SDWriter::closeWave() took %dms.\n", wt);
  }
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
    if (Verbose > 0) {
      Serial.printf("ERROR in SDWriter::write(): data overrun! Missed %d samples (%.0f%% of buffer, %.0fms).\n", missed, 100.0*missed/Data->nbuffer(), 1000*Data->time(missed));
      unsigned long wt = WriteTime;
      Serial.printf("----> last write %dms ago.\n", wt);
    }
    return -4;
  }
  if (available() == 0) {
    if (writeTime() > 4*Data->DMABufferTime()) {
      if (Verbose > 0) {
	Serial.println("ERROR in SDWriter::write(): no data are produced!");
	Serial.printf("   SD cycle: %5d,  SD index: %6d\n", Cycle, Index);
	Serial.printf("  TDM cycle: %5d, TDM index: %6d\n", Data->cycle(), Data->index());
      }
      return -3;
    }
    else
      return 0;
  }
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = WriteTime;
    Serial.printf("----> SDWriter::write(): last write %dms ago.\n", wt);
  }
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
      if (Verbose > 1 && WriteTime > MaxWriteTime) {
	unsigned long wt = WriteTime;
	Serial.printf("----> SDWriter::write(): needed %dms for writing end-of-buffer data to SD card.\n", wt);
      }
      WriteTime = 0;
      samples0 = nbytes / sizeof(sample_t);
      increment(samples0);
      FileSamples += samples0;
      if (samples0 < nwrite) {
	if (Verbose > 0)
	  Serial.printf("WARNING in SDWriter::write(): only wrote %d samples of %d to the end of the data buffer\n", samples0, nwrite);
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
    if (Verbose > 0 && samples1 < nwrite)
      Serial.printf("WARNING in SDWriter::write(): only wrote %d samples of %d\n", samples1, nwrite);
    increment(samples1);
    FileSamples += samples1;
  }
  if (Verbose > 1 && WriteTime > MaxWriteTime) {
    unsigned long wt = WriteTime;
    Serial.printf("----> SDWriter::write(): needed %dms for writing beginning-of-buffer data to SD card.\n", wt);
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


void SDWriter::start(const SDWriter &file) {
  synchronize(file);
  WriteTime = 0;
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


void SDWriter::setMaxWriteTime(uint ms) {
  MaxWriteTime = ms;
}

