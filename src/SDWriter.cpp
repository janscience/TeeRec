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


bool SDWriter::open(const char *fname) {
  if (! SDAvailable || strlen(fname) == 0)
    return false;
  if (File.isOpen()) {
    Serial.println("failed to open file because a file is still open.");
    return false;
  }
  if (!File.open(fname, O_WRITE | O_CREAT))
    Serial.printf("failed to open file %s\n", fname);
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

