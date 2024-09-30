/*
  SDCard - Oparate on SD cards.
  Created by Jan Benda, September 26th, 2024.
*/

#ifndef SDCard_h
#define SDCard_h


#include <Arduino.h>
#include <SD.h>
#include <SPI.h>


class SDCard : public SDClass {

 public:

  // Initialize. You need to call begin() for accessing the SD card.
  // The optional name is used for error messages.
  SDCard(const char *name=0);
  // End usage of SD card.
  virtual ~SDCard();

#ifdef BUILTIN_SDCARD
  // Initialize built in SD card on SDIO bus.
  // Return true on success.
  bool begin();
#endif
  
  // Initialize SD card on specified SPI chip select pin.
  // Set cs to 255 for not initializing the SD card.
  // Return true on success.
  bool begin(uint8_t cs);
  
  // Initialize SD card on SPI bus.
  // cs is the chip select pin,
  // set it to 255 for not initializing the SD card.
  // opt is DEDICATED_SPI (fast) or SHARED_SPI (a bit slower),
  // clock is the clock frequency in MHZ (max 50),
  // spi is the SPI bus, &SPI or &SPI1.
  // e.g. for the first SPI bus use:
  //   begin(10, DEDICATED_SPI, 24, &SPI);
  // Return true on success.
  bool begin(uint8_t cs, uint8_t opt, uint32_t clock, SPIClass* spi);
  
  // End usage of SD card.
  void end();

  // Begin SD card usage with the same begin() command as before.
  bool restart();

  // Availability of a SD card. 
  bool available() const { return Available; };

  // Check availability of SD card.
  // If not present, print out error message on stream.
  bool checkAvailability(Stream &stream=Serial);

  // The name of the SD card, as passed to the constructor.
  const char *name() const { return Name; };

  // True if SD card is busy.
  bool isBusy();

  // Check, whether SD card is available, has enough free space (in
  // Bytes) and can be written.
  // If not, it tries to restart the SD card.
  bool check(float minfree=1024*1024, Stream &stream=Serial);

  // Make directory if it does not exist and
  // make it the currrent working directory.
  // Return true on succes.
  bool dataDir(const char *path);

  // Reset current working directory to root.
  // Return true on succes.
  bool rootDir();

  // List all files in path (non-recursively).
  // If list_dirs, then also list directories.
  // If list_sizes, then also print out file sizes in bytes.
  void listFiles(const char *path, bool list_dirs=false, bool list_sizes=false,
		 Stream &stream=Serial);

  // Remove all files in path (non-recursively).
  void removeFiles(const char *path, Stream &stream=Serial);

  // Write the card type in types, that should hold at least 8 characters.
  void cardType(char *types);

  // Capacity of the card in Bytes.
  float capacity();

  // Free space of the card in Bytes.
  float free();

  // Serial number of the SD card as hex string (minimum size of 9 characters).
  void serial(char *s);

  // Report SD card infos, capacity and available space.
  void report(Stream &stream=Serial);

  // Run a benchmark test and report data rates for writing and reading.
  // Write and read buffers of size bufer_size bytes
  // of a file_size MB large file for repeats times.
  void benchmark(size_t buffer_size=512, uint32_t file_size=10,
		 int repeats=2, Stream &stream=Serial);

  // Flash erase all data.
  void erase(Stream &stream=Serial);
  
  // Format the SD card, but keep the specified (small) file.
  // If erase_card, flash erase all data first.
  void format(const char *path=0, bool erase_card=false,
	      Stream &stream=Serial);

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Reset the counter that is used by incrementFileName().
  // Call it, whenever the filename changes, for example, because of a new date.
  void resetFileCounter();

  // Open file on SD card for reading.
  FsFile openRead(const char *path);

  // Open file on SD card for writing (not appending).
  FsFile openWrite(const char *path);

  // Open file on SD card for appending to existing file.
  FsFile openAppend(const char *path);

  
 protected:

  static const size_t MaxName = 32;
  char Name[MaxName];

  bool Available;
  int SDIOCSPin;

  uint16_t NameCounter;
};


#endif
