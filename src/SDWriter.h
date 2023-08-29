/*
  SDWriter - library for writing data from a DataWorker to SD card.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <DataWorker.h>
#include <WaveHeader.h>


// Add a bit of functionality to SDClass.
class SDCard : public SDClass {

 public:

  // Initialize. You need to call begin() for accessing the SD card.
  SDCard();
  // End usage of SD card.
  virtual ~SDCard();

#ifdef BUILTIN_SDCARD
  // Initialize built in SD card.
  // Return true on success.
  bool begin();
#endif
  
  // Initialize SD card on specified SPI chip select pin.
  // Return true on success.
  bool begin(uint8_t csPin);
  
  // End usage of SD card.
  void end();

  // Availability of a SD card. 
  bool available() const { return Available; };

  // True if SD card is busy.
  bool isBusy();

  // Make directory if it does not exist and
  // make it the currrent working directory.
  // Return true on succes.
  bool dataDir(const char *path);

  // Reset current working directory to root.
  // Return true on succes.
  bool rootDir();

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Reset the counter that is used by incrementFileName().
  // Call it, whenever the filename changes, for example, because of a new date.
  void resetFileCounter();

  // Remove all files in path (non-recursively).
  void removeFiles(const char *path);

  // Flash erase all data.
  void erase();
  
  // Format the SD card, but keep the specified (small) file.
  // If erase_card, flash erase all data first.
  void format(const char *path=0, bool erase_card=false);

  // Open file on SD card for reading.
  File openRead(const char *path);

  // Open file on SD card for writing (not appending).
  File openWrite(const char *path);

  // Open file on SD card for appending to existing file.
  File openAppend(const char *path);

  
 protected:

  bool Available;

  uint16_t NameCounter;
};


// Write data from buffer into wave files.
class SDWriter : public DataWorker {

 public:

  // Size of blocks for writing.
  // Must be an integer multiple of InputADC::MajorSize.
  static const size_t MajorSize = 512;

  // Initialize writer on default SD card.
  SDWriter(const DataWorker &data);
  // Initialize writer on SD card.
  SDWriter(SDCard &sd, const DataWorker &data);
  // Close file and end usage of SD card.
  ~SDWriter();

  // Availability of a SD card. 
  bool cardAvailable() const;

  // The SD card volume on which data are written.
  SDCard *sdcard() { return SDC; };

  // End usage of SD card if it was created by SDWriter.
  void end();

  // Return write interval in seconds.
  float writeInterval() const;

  // Set write interval depending on settings of the DataWorker.
  // If time is positive it is a time interval in seconds.
  // If time is negative it is the fraction of the full data buffer.
  void setWriteInterval(float time=-0.25);

  // Return time after last write in seconds.
  float writeTime() const;

  // True if data are pending that need to be written to file.
  // Check this regularly in loop() and call write() if true is returned.
  bool pending();

  // Open new file for writing.
  // fname is the name of the file inclusively extension.
  bool open(const char *fname);

  // True if file is open.
  bool isOpen() const;

  // Close file.
  void close();

  // Return file object.
  File &file() { return DataFile; };

  // Open new file for writing and write wave header with metadata
  // from all data producers.
  // For samples<0, take max file size.
  // For samples=0, initialize wave header with unspecified size.  You
  // then need to close the file with closeWave() and provide the
  // number of samples there.
  // If no file extension is provided, ".wav" is added.
  // Return true if the file was successfully opened.
  bool openWave(const char *fname, int32_t samples=-1,
                const char *datetime=0);

  // Update wave header with proper file size and close file.
  // Return true if the file was not open or the file was sucessfully
  // closed, including an update of the wave header with the actual
  // file size.
  bool closeWave();

  // Name of the currently or previously open file.
  const String &name() const {return FileName; };

  // Basename of the currently or previously open file (without extension).
  String baseName() const;

  // Return wave header. 
  WaveHeader &header() { return Wave; };

  // Write available data to file (if the file is open).
  // If maxFileSamples() is set (>0), then stop writing after that
  // many samples.
  // Return number of written samples or a negative number on error:
  //  0: no data available yet.
  // -1: file is not open.
  // -2: file is already full according to maxFileSamples().
  // -3: no data are available, although there should be some.
  // -4: overrun.
  // -5: data were not written to file (disk full?)
  ssize_t write();

  // Start writing to a file from the current sample minus decr samples on.
  void start(size_t decr=0);

  // Return current file size in samples.
  size_t fileSamples() const;

  // Return current file size in seconds.
  float fileTime() const;

  // Return current file size as a string displaying minutes and seconds.
  // str must hold at least 6 characters.
  void fileTimeStr(char *str) const;

  // Set maximum file size to a fixed number of samples modulo 256.
  void setMaxFileSamples(size_t samples);

  // Set maximum file size to approximately that many seconds.
  void setMaxFileTime(float secs);

  // Return actually used maximum file size in samples.
  size_t maxFileSamples() const;

  // Return maximum file size in seconds.
  float maxFileTime() const;

  // Return true if maximum number of samples have been written
  // and a new file needs to be opened.
  bool endWrite();
  
  // Data buffer has been initialized.
  virtual void reset();
  

 protected:

  SDCard *SDC;
  bool SDOwn;
  mutable File DataFile;   // mutable because File from FS.h has non-constant bool() function
  String FileName;         // name of the currently or previously open file.

  WaveHeader Wave;

  elapsedMillis WriteTime;
  uint WriteInterval;

  size_t FileSamples;    // current number of samples stored in the file.
  size_t FileMaxSamples; // maximum number of samples to be stored in a file.
  
};


#endif
