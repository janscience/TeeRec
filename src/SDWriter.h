/*
  SDWriter - Write data from a DataWorker to SD card.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <DataWorker.h>
#include <SDCard.h>
#include <WaveHeader.h>


class SDWriter : public DataWorker {

 public:

  // Size of blocks for writing.
  // Must be an integer multiple of InputADC::MajorSize.
  static const size_t MajorSize = 512;

  // Initialize a non-writer.
  SDWriter();
  // Initialize writer on default SD card.
  SDWriter(const DataWorker &data, int verbose=0);
  // Initialize writer on SD card.
  SDWriter(SDCard &sd, const DataWorker &data, int verbose=0);
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

  // Set write interval.
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
  FsFile &file() { return DataFile; };

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

  // Open new file for writing and write wave header from file.
  // Return true if the file was successfully opened.
  bool openWave(const char *fname, const WaveHeader &wave);

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
  
  // Start writing from the same sample as file.
  void start(const SDWriter &file);

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
  
  // Set maximum time for write operations in milliseconds.
  // If the write operation takes longer and the verbosity level is 2 or higher,
  // then print the elepsed time on Serial.
  void setMaxWriteTime(uint32_t ms);

  // Time when writing was started in milliseconds.
  uint32_t startWriteTime() const { return StartWriteTime; };

  
 protected:

  // Print error messages about timing issues, depending on verbosity level.
  void checkTiming(uint32_t t, const char *function, const char *message);

  SDCard *SDC;
  bool SDOwn;
  mutable FsFile DataFile;   // mutable because File from FS.h has non-constant bool() function
  String FileName;           // name of the currently or previously open file.

  WaveHeader Wave;

  elapsedMillis WriteTime;
  uint32_t MaxWriteTime;
  uint32_t WriteInterval;

  size_t FileSamples;    // current number of samples stored in the file.
  size_t FileMaxSamples; // maximum number of samples to be stored in a file.

  uint32_t StartWriteTime; // time when writing was started in milliseconds.
  
};


#endif
