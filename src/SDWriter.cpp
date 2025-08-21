#include <DataBuffer.h>
#include <SDWriter.h>


SDWriter::SDWriter() :
  DataWorker(),
  SDC(NULL),
  SDOwn(true),
  FileName(""),
  WriteTime(0),
  MaxWriteTime(100),
  WriteInterval(100),
  FileSamples(0),
  FileMaxSamples(0),
  StartWriteTime(0) {
  DataFile.close();
}


SDWriter::SDWriter(const DataWorker &producer, int verbose) :
  DataWorker(&producer, verbose),
  SDC(NULL),
  SDOwn(true),
  FileName(""),
  WriteTime(0),
  MaxWriteTime(100),
  WriteInterval(100),
  FileSamples(0),
  FileMaxSamples(0),
  StartWriteTime(0) {
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
  FileMaxSamples(0),
  StartWriteTime(0) {
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
    WriteInterval = uint(-1000*time*bufferTime()); // fraction of the buffer
  else
    WriteInterval = uint(1000*time);               // time interval in seconds
  if (0.001*WriteInterval > 0.5*bufferTime())
    Serial.println("WARNING! SDWriter::setWriteInterval() interval larger than half the buffer!");
}


float SDWriter::writeTime() const {
  return 0.001*WriteTime;
}


void SDWriter::checkTiming(uint32_t t, const char *function,
			   const char *message) {
  if ((Verbose > 1 && t > MaxWriteTime) ||
      0.001*t > 0.5*bufferTime()) {
    if (0.001*t > 0.5*bufferTime())
      Serial.print("WARNING");
    else
      Serial.print("------>");
    Serial.printf(" in SDWriter::%s() on %sSD card: ",
		 function, sdcard()->name());
    Serial.printf(message, t);
    Serial.println(".");
  }
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
    Serial.printf("failed to open file \"%s\", because the file is still open.\n", fname);
    return false;
  }
  FileName = fname;
  DataFile = SDC->openWrite(fname);
  FileSamples = 0;
  checkTiming(t, "open", "opening file took %lums");
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
  elapsedMillis t = 0;
  if (samples < 0)
    samples = FileMaxSamples;
  Wave.setFormat(nchannels(), rate(), resolution(), dataResolution());
  char gs[16];
  gainStr(gs, 16);
  Wave.setGain(gs);
  setWaveHeader(Wave);    // recursively calls setWaveHeader on all producers.
  Wave.setData(samples);
  if (datetime != 0)
    Wave.setDateTime(datetime);
  else
    Wave.clearDateTime();
  Wave.assemble();
  if (DataFile.write(Wave.Buffer, Wave.NBuffer) != Wave.NBuffer) {
    Serial.printf("ERROR: initial writing of wave header failed on %sSD card.\n", sdcard()->name());
    return false;
  }
  checkTiming(t, "openWave", "opening wave file took %lums");
  return (DataFile) ? true : false;
}


bool SDWriter::openWave(const char *fname, const WaveHeader &wave) {
  if (!open(fname))
    return false;
  elapsedMillis t = 0;
  if (DataFile.write(wave.Buffer, wave.NBuffer) != wave.NBuffer) {
    Serial.printf("ERROR: initial writing of wave header failed on %sSD card.\n",
		  sdcard()->name());
    return false;
  }
  checkTiming(t, "openWave", "opening wave file took %lums");
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
      Serial.printf("ERROR: final writing of wave header on %sSD card failed.\n", sdcard()->name());
      success = false;
    }
  }
  close();
  checkTiming(t, "closeWave", "closing wave file took %lums");
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
    uint32_t wt = WriteTime;
    Serial.printf("ERROR in SDWriter::write() on %sSD card: data overrun! Missed %d samples (%.0f%% of buffer, %.0fms).\n", sdcard()->name(), missed, 100.0*missed/nbuffer(), 1000*time(missed));
    Serial.printf("------> last write on %sSD card %dms ago.\n", sdcard()->name(), wt);
    return -4;
  }
  if (available() == 0) {
    if (writeTime() > 4*Data->DMABufferTime()) {
      Serial.printf("ERROR in SDWriter::write() on %sSD card: no data are produced!\n", sdcard()->name());
      if (Verbose > 0) {
	Serial.printf("    Worker cycle: %u,   Worker index: %u\n", Cycle, Index);
	Serial.printf("  Producer cycle: %u, Producer index: %u, Buffer size: %u\n", Data->cycle(), Data->index(), Data->nbuffer());
      }
      return -3;
    }
    else
      return 0;
  }
  checkTiming(WriteTime, "write", "last write %lums ago");
  WriteTime = 0;
  size_t index = Producer->index();
  size_t nwrite = 0;
  if (Index >= index) {
    nwrite = nbuffer() - Index;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    if (nwrite > 0) {
      nbytes = DataFile.write((void *)&Data->buffer()[Index],
			      sizeof(sample_t)*nwrite);
      if (nbytes == 0)
	return -5;
      checkTiming(WriteTime, "write", "needed %lums for writing end-of-buffer data");
      samples0 = nbytes / sizeof(sample_t);
      increment(samples0);
      FileSamples += samples0;
      if (samples0 < nwrite) {
	if (Verbose > 0)
	  Serial.printf("WARNING in SDWriter::write() on %sSD card: only wrote %d samples of %d to the end of the data buffer\n",
			sdcard()->name(), samples0, nwrite);
	return samples0;
      }
      if (FileMaxSamples > 0 && FileSamples >= FileMaxSamples)
	return samples0;
      index = Producer->index();
    }
  }
  WriteTime = 0;
  nwrite = index - Index;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  nwrite = (nwrite/MajorSize)*MajorSize;          // write only full blocks
  if (nwrite > 0) {
    nbytes = DataFile.write((void *)&Data->buffer()[Index],
			    sizeof(sample_t)*nwrite);
    if (nbytes == 0)
      return -5;
    WriteTime = 0;
    samples1 = nbytes / sizeof(sample_t);
    if (Verbose > 0 && samples1 < nwrite)
      Serial.printf("WARNING in SDWriter::write() on %sSD card: only wrote %d samples of %d\n",
		    sdcard()->name(), samples1, nwrite);
    increment(samples1);
    FileSamples += samples1;
  }
  checkTiming(WriteTime, "write", "needed %lums for writing beginning-of-buffer data");
  return samples0 + samples1;
}


void SDWriter::start(size_t decr) {
  if (!synchronize())
    Serial.println("ERROR in SDWriter::startWrite(): data buffer not initialized yet. ");
  WriteTime = 0;
  StartWriteTime = millis();
  if (decr > 0) {
    decrement(decr);
    int decms = int(1000.0*time(decr));
    WriteTime += decms;
    StartWriteTime -= decms;
  }
}


void SDWriter::start(const SDWriter &file) {
  synchronize(file);
  WriteTime = 0;
  StartWriteTime = file.startWriteTime();
}


size_t SDWriter::fileSamples() const {
  return FileSamples;
}


float SDWriter::fileTime() const {
  return time(FileSamples);
}


void SDWriter::fileTimeStr(char *str) const {
  timeStr(FileSamples, str);
}


void SDWriter::setMaxFileSamples(size_t samples) {
  FileMaxSamples = (samples/MajorSize)*MajorSize;
}


void SDWriter::setMaxFileTime(float secs) {
  if (rate() == 0)
    Serial.println("WARNING in SDWriter::setMaxFileTime(): sampling rate not yet set!");
  setMaxFileSamples(samples(secs));
}


size_t SDWriter::maxFileSamples() const {
  return FileMaxSamples;
}


float SDWriter::maxFileTime() const {
  return time(FileMaxSamples);
}


bool SDWriter::endWrite() {
  return (FileMaxSamples > 0 && FileSamples >= FileMaxSamples);
}


void SDWriter::reset() {
  DataWorker::reset();
  FileSamples = 0;
}


void SDWriter::setMaxWriteTime(uint32_t ms) {
  MaxWriteTime = ms;
}

