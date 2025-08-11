#include <SDCard.h>


SDCard::SDCard(const char *name) :
  Available(false),
  SDIOCSPin(-1),
  NameCounter(0) {
  if (name == 0)
    strcpy(Name, "");
  else {
    strcpy(Name, name);
    strcat(Name, " ");
  }
  strcpy(WorkingDir, "");
}


SDCard::~SDCard() {
  end();
}


#ifdef BUILTIN_SDCARD
bool SDCard::begin() {
  return begin(BUILTIN_SDCARD);
}
#endif


bool SDCard::begin(uint8_t cs) {
  Available = false;
  SDIOCSPin = cs;
  if (cs == 255)
    return false;
  if (!SDClass::begin(cs))
    return false;
  Available = true;
  sdfs.chvol();
  return true;
}


bool SDCard::begin(uint8_t cs, uint8_t opt, uint32_t clock, SPIClass* spi) {
  Available = false;
  SDIOCSPin = -1;
  if (cs == 255)
    return false;
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


bool SDCard::restart() {
  end();
  if (SDIOCSPin >= 0)
    begin(SDIOCSPin);
  else {
    if (!sdfs.restart())
      return false;
    sdfs.chvol();
    Available = true;
  }
  return Available;
}


bool SDCard::checkAvailability(Stream &stream) {
  if (! Available) {
    stream.printf("! ERROR: No %sSD card present.\n", Name);
    stream.println();
    return false;
  }
  return true;
}


bool SDCard::isBusy() {
  return sdfs.isBusy();
}


bool SDCard::check(float minfree, Stream &stream) {
  if (minfree < 512)
    minfree = 512.0;
  for (int k=0; k<2; k++) {
    if (k == 1) {
      restart();
      stream.printf("Restarted %sSD card.\n", Name);
    }
    if (! Available) {
      if (k == 0)
	continue;
      stream.printf("! ERROR: No %sSD card present.\n\n", Name);
      return false;
    }
    if (free() < minfree) {
      if (k == 0)
	continue;
      stream.printf("! WARNING: No space left on %sSD card.\n\n", Name);
      return false;
    }
    FsFile tf = sdfs.open("testit.wrt", O_RDWR | O_CREAT | O_TRUNC);
    if (! tf) {
      if (k == 0)
	continue;
      stream.printf("! ERROR: Can not write onto %sSD card.\n\n", Name);
      return false;
    }
    tf.close();
    if (! remove("testit.wrt")) {
      if (k == 0)
	continue;
      stream.printf("! ERROR: Failed to remove test file on %s SD card.\n\n", Name);
      return false;
    }
    stream.printf("%sSD card present and writable.\n\n", Name);
    return true;
  }
  return false;
}


bool SDCard::dataDir(const char *path) {
  if (! Available)
    return false;
  sdfs.chvol();
  const char *npath = path;
  char *num = strstr(path, "NUM");
  if (num == NULL) {
    if (! exists(npath))
      mkdir(npath);
  }
  else {
    for (int i=1; i<=99; i++) {
      *num = '\0';
      char new_path[MaxDir];
      snprintf(new_path, MaxDir, "%s%02d%s", path, i, num + 3);
      new_path[MaxDir - 1] = '\0';
      npath = new_path;
      if (! exists(npath)) {
	mkdir(npath);
	break;
      }
    }
  }
  NameCounter = 0;
  bool r = sdfs.chdir(npath);
  if (r) {
    strncpy(WorkingDir, npath, MaxDir);
    WorkingDir[MaxDir - 1] = '\0';
  }
  return r;
}


bool SDCard::rootDir() {
  if (! Available)
    return false;
  sdfs.chvol();
  bool r = sdfs.chdir("/");
  if (r)
    strcpy(WorkingDir, "/");
  return r;
}


void SDCard::listFiles(const char *path, bool list_dirs, bool list_sizes,
		       Stream &stream) {
  if (!checkAvailability(stream))
    return;
  
  SdFile file;
  FsFile dir = sdfs.open(path);
  if (!dir) {
    stream.printf("Folder \"%s\" does not exist on %s SD card.\n", path, Name);
    return;
  }
  stream.printf("Files in \"%s\" on %sSD card:\n", path, Name);
  float file_sizes = 0.0;
  int n = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    char fname[200];
    file.getName(fname, 200);
    if (!file.isDir()) {
      if (n == 0 && list_sizes)
	stream.println("  size (bytes) name");
      stream.print("  ");
      if (list_sizes) {
	stream.printf("%12lu ", file.fileSize());
	file_sizes += 1e-6*file.fileSize();
      }
      stream.println(fname);
      n++;
    }
    else if (list_dirs) {
      if (n == 0 && list_sizes)
	stream.println("  size (bytes) name");
      stream.print("  ");
      if (list_sizes) {
	stream.printf("%12lu ", file.fileSize());
	file_sizes += 1e-6*file.fileSize();
      }
      stream.print(fname);
      stream.println("/");
      n++;
    }
  }
  if (n == 0)
    stream.printf("No files found.\n");
  else {
    if (n > 1)
      stream.printf("%d files found", n);
    else
      stream.printf("%d file found", n);
    if (list_sizes)
      stream.printf(" (%.3f MB).\n", file_sizes);
    else
      stream.println(".");
  }
}


void SDCard::removeFiles(const char *path, Stream &stream) {
  if (!checkAvailability(stream))
    return;
  
  SdFile file;
  FsFile dir = sdfs.open(path);
  if (!dir) {
    stream.printf("Folder \"%s\" does not exist on %sSD card.\n", path, Name);
    return;
  }
  stream.printf("Erase all files in \"%s\" on %sSD card:\n", path, Name);
  int n = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      if (dir.remove(fname)) {
	stream.print("  erased ");
	stream.println(fname);
	n++;
      }
      else {
	stream.print("  Failed to erase file ");
	stream.println(fname);
      }
    }
  }
  if (n == 0)
    stream.println("No files found.");
  else if (n == 1)
    stream.printf("Removed %d file.\n", n);
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
  if (!checkAvailability(stream))
    return;
  
  char types[16];
  cardType(types);

  char files[16];
  int ft = sdfs.fatType();
  if (ft > 32)
    strcpy(files, "exFAT");
  else
    sprintf(files, "FAT%d", ft);
  
  float cap = capacity();
  if (cap < 1.0) {
    stream.printf("! ERROR: Failed to get sector count of %sSD card.\n", Name);
    return;
  }
  
  cid_t cid;
  if (!sdfs.card()->readCID(&cid)) {
    stream.printf("! ERROR: Failed to read CID from %sSD card.\n", Name);
    return;
  }
  
  stream.printf("%sSD card:\n", Name);
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
  if (!checkAvailability(stream))
    return;
  
  // adapted from bench.ino example of the sdfat library

  const bool pre_allocate = true;

  // open or create file - truncate existing file:
  FsFile file = sdfs.open("bench.dat", O_RDWR | O_CREAT | O_TRUNC);
  if (!file) {
    stream.printf("! ERROR: Failed to create 'bench.dat' file on %sSD card.\n\n", Name);
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

  stream.printf("Benchmarking write and read speeds of %sSD card\n", Name);
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
  
  stream.printf("Erase %sSD card:\n", Name);
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
  if (! Available) {
    stream.printf("! ERROR: No %sSD card present.\n", Name);
    stream.println();
    return;
  }
  
  FsFile file;
  size_t n = 10;
  // read file:
  if (path != 0 && strlen(path) > 0) {
    stream.printf("Read file \"%s\" on %sSD card ...\n", path, Name);
    sdfs.chdir();
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
  stream.printf("Format %sSD card:\n", Name);
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
  // make new file system known:
  sdfs.card()->syncDevice();
  restart();
  stream.println();
  // write file:
  if (path != 0 && strlen(path) > 0) {
    file = openWrite(path);
    file.write(buffer, n);
    file.close();
    stream.printf("Restored file \"%s\" on %sSD card.\n", path, Name);
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
	  Serial.printf("WARNING: file name overflow on %sSD card.\n", Name);
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
	  Serial.printf("WARNING: file name overflow on %sSD card.\n", Name);
	  return "";
	}
	volatile int nn_size = sizeof(nn); // avoid truncation warning: https://stackoverflow.com/a/70938456
	snprintf(nn, nn_size, "%0*d", width, NameCounter);
	name.replace(nums, nn);
      }
    } while (sdfs.exists(name.c_str()));
    return name;
  }
  else
    return fname;
}

    
void SDCard::resetFileCounter() {
  NameCounter = 0;
}


FsFile SDCard::openRead(const char *path) {
  return sdfs.open(path, O_READ);
}


FsFile SDCard::openWrite(const char *path) {
  return sdfs.open(path, O_WRONLY | O_CREAT | O_TRUNC );
}


FsFile SDCard::openAppend(const char *path) {
  return sdfs.open(path, O_WRONLY | O_CREAT | O_AT_END);
}

