#include <SDCard.h>


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


bool SDCard::begin(uint8_t cs, uint8_t opt, uint32_t clock, SPIClass* spi) {
  if (!sdfs.begin(SdSpiConfig(cs, opt, SD_SCK_MHZ(clock), spi)))
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


bool SDCard::check(float minfree, Stream &stream) {
  if (minfree < 512)
    minfree = 512.0;
  for (int k=0; k<2; k++) {
    if (k == 1) {
      end();
      sdfs.restart();
    }
    if (! Available) {
      if (k == 0)
	continue;
      stream.println("! ERROR: No SD card present.\n");
      return false;
    }
    if (free() < minfree) {
      if (k == 0)
	continue;
      stream.println("! WARNING: No space left on SD card.\n");
      return false;
    }
    File tf = open("testit.wrt", FILE_WRITE);
    if (! tf) {
      if (k == 0)
	continue;
      stream.println("! ERROR: Can not write onto SD card.\n");
      return false;
    }
    tf.close();
    if (! remove("testit.wrt")) {
      if (k == 0)
	continue;
      stream.println("! ERROR: Failed to remove test file.\n");
      return false;
    }
    stream.println("SD card present and writable.\n");
    return true;
  }
  return false;
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


void SDCard::listFiles(const char *path, bool list_dirs, Stream &stream) {
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
  while (file.openNext(&dir, O_RDONLY)) {
    char fname[200];
    file.getName(fname, 200);
    if (!file.isDir()) {
      stream.print("  ");
      stream.println(fname);
      n++;
    }
    else if (list_dirs) {
      stream.print("  ");
      stream.print(fname);
      stream.println("/");
      n++;
    }
  }
  if (n > 1)
    stream.printf("%d files found.\n", n);
  else if (n == 1)
    stream.printf("%d file found.\n", n);
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
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      char pname[200];
      strcpy(pname, path);
      if (pname[strlen(pname) - 1] != '/')
	strcat(pname, "/");
      strcat(pname, fname);
      if (dir.remove(pname)) {
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
    stream.printf("Removed %d files.\n", n);
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


void SDCard::serial(char *s) {
  cid_t cid;
  if (!sdfs.card()->readCID(&cid)) {
    s[0] = '\0';
    return;
  }
  sprintf(s, "%lx", cid.psn);
}


void SDCard::report(Stream &stream) {
  if (! Available) {
    stream.println("! ERROR: No SD card present.");
    stream.println();
    return;
  }
  
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
  
  cid_t cid;
  if (!sdfs.card()->readCID(&cid)) {
    stream.println("! ERROR: Failed to read CID from SD card.");
    return;
  }
  
  stream.println("SD card:");
  stream.printf("  Manufacturer ID   : %x\n", cid.mid);
  stream.printf("  OEM ID            : %c%c\n", cid.oid[0], cid.oid[1]);
  stream.printf("  Product           : %c%c%c%c%c\n", cid.pnm[0],
		cid.pnm[1], cid.pnm[2], cid.pnm[3], cid.pnm[4]);
  stream.printf("  Version           : %d.%d\n", cid.prv_n, cid.prv_m);
  stream.printf("  Serial number     : %lx\n", cid.psn);
  stream.printf("  Manufacturing date: %d/%d\n", cid.mdt_month,
		2000 + cid.mdt_year_low + 10*cid.mdt_year_high);
  stream.printf("  Type              : %s\n", types);
  stream.printf("  File system       : %s\n", files);
  stream.printf("  Capacity          : %.3f GB\n", 1e-9 * cap);
  stream.printf("  Available         : %.3f GB\n", 1e-9 * free());
  stream.println();
}


void SDCard::benchmark(size_t buffer_size, uint32_t file_size, int repeats,
		       Stream &stream) {
  // adapted from bench.ino example of the sdfat library

  const bool pre_allocate = true;

  // open or create file - truncate existing file:
  FsFile file = sdfs.open("bench.dat", O_RDWR | O_CREAT | O_TRUNC);
  if (!file) {
    stream.println("! ERROR: Failed to create 'bench.dat' file on SD card.\n");
    return;
  }
  if (buffer_size < 32) {
    stream.println("! ERROR: Buffer size too small.\n");
    return;
  }
  
  // insure 4-byte alignment.
  uint32_t buf32[(buffer_size + 3)/4];
  uint8_t* buf = (uint8_t*)buf32;

  // fill buffer with known data:
  for (size_t i=0; i<buffer_size - 2; i++) {
    buf[i] = 'A' + (i % 26);
  }
  buf[buffer_size - 2] = '\r';
  buf[buffer_size - 1] = '\n';

  stream.println("Benchmarking write and read speeds.");
  stream.println("- 'speed' is the average data rate for writing/reading the whole file.");
  stream.println("- 'latency' is the average, minimum, and maximum time it takes to write/read a single buffer.");
  stream.printf("- file   size: %dMB\n", file_size);
  stream.printf("- buffer size: %dBytes\n\n", buffer_size);

  file_size *= 1000000UL;   // to bytes

  // run write test:
  stream.println("Write speed and latency:");
  stream.println("speed\tavg\tmin\tmax");
  stream.println("MB/s\tms\tms\tms");
  uint32_t n = file_size/buffer_size;
  for (int k=0; k<repeats; k++) {
    file.truncate(0);
    if (pre_allocate) {
      if (!file.preAllocate(file_size))
	stream.println("! ERROR: pre-allocation of file failed.\n");
    }
    uint32_t max_latency = 0;
    uint32_t min_latency = 9999999;
    uint32_t total_latency = 0;
    bool skip = true;
    uint32_t t = millis();
    for (uint32_t i=0; i<n; i++) {
      uint32_t m = micros();
      if (file.write(buf, buffer_size) != buffer_size)
	stream.println("! ERROR: writing to file failed.\n");
      m = micros() - m;
      total_latency += m;
      if (skip) {
        // Wait until first write to SD, not just a copy to the cache.
        skip = file.curPosition() < 512;
      } else {
        if (max_latency < m)
          max_latency = m;
        if (min_latency > m)
          min_latency = m;
      }
    }
    file.sync();
    t = millis() - t;
    float s = file.fileSize();
    stream.printf("%.2f\t%.3f\t%.3f\t%.3f\n", 0.001*s/t, 0.001*total_latency/n,
		  0.001*min_latency, 0.001*max_latency);
  }
  stream.println();

  // run read test:
  stream.println("Read speed and latency:");
  stream.println("speed\tavg\tmin\tmax");
  stream.println("MB/s\tms\tms\tms");
  for (int k=0; k<repeats; k++) {
    file.rewind();
    uint32_t max_latency = 0;
    uint32_t min_latency = 9999999;
    uint32_t total_latency = 0;
    bool skip = true;
    uint32_t t = millis();
    for (uint32_t i=0; i<n; i++) {
      buf[buffer_size - 1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(buf, buffer_size);
      m = micros() - m;
      if (nr != (int32_t)buffer_size)
	stream.println("! ERROR: reading from file failed.\n");
      total_latency += m;
      if (buf[buffer_size-1] != '\n')
	stream.println("! ERROR: data check failed.\n");
      if (skip)
        skip = false;
      else {
        if (max_latency < m)
          max_latency = m;
        if (min_latency > m)
          min_latency = m;
      }
    }
    float s = file.fileSize();
    t = millis() - t;
    stream.printf("%.2f\t%.3f\t%.3f\t%.3f\n", 0.001*s/t, 0.001*total_latency/n,
		  0.001*min_latency, 0.001*max_latency);
  }
  stream.println();
  file.close();
  if (!sdfs.remove("bench.dat"))
    stream.println("Failed to remove 'bench.dat'");
  stream.println("Done");
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
  if (path != 0 && strlen(path) > 0) {
    stream.printf("Read file \"%s\" ...\n", path);
    rootDir();
    file = openRead(path);
    n = file.available();
  }
  char buffer[n];
  if (path != 0 && strlen(path) > 0) {
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
  if (path != 0 && strlen(path) > 0) {
    file = openWrite(path);
    file.write(buffer, n);
    file.close();
    stream.printf("Restored file \"%s\".\n", path);
    stream.println();
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

