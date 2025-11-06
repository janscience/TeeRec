#include <TimeLib.h>
#include <SDCard.h>


// needed by SdFat to set creation/modification/access times
void SDCardDateTime(uint16_t* date, uint16_t* time) {
  time_t t = now();
  *date = FS_DATE(year(t), month(t), day(t));
  *time = FS_TIME(hour(t), minute(t), second(t));
}


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


bool SDCard::isBusy() {
  return sdfs.isBusy();
}


bool SDCard::dataDir(const char *path, bool new_dir) {
  if (! Available)
    return false;
  char old_path[MaxDir];
  char new_path[MaxDir];
  sdfs.chvol();
  const char *opath = path;
  const char *npath = path;
  char *num = strstr(opath, "NUM");
  if (num == NULL) {
    if (! exists(npath)) {
      SdFile::dateTimeCallback(SDCardDateTime);
      mkdir(npath);
    }
    else if (new_dir) {
      strncpy(old_path, opath, MaxDir);
      old_path[MaxDir - 1] = '\0';
      strcat(old_path, "-NUM1");
      opath = old_path;
      num = strstr(opath, "NUM");
    }
  }
  if (num != NULL) {
    int offs = 3;
    int width = 2;
    if (isdigit(*(num + 3))) {
      width = *(num + 3) - '0';
      offs = 4;
    }
    for (int i=1; i<=99; i++) {
      size_t n = num - opath;
      memcpy(new_path, opath, n);
      snprintf(new_path + n, MaxDir - n, "%0*d%s", width, i, num + offs);
      new_path[MaxDir - 1] = '\0';
      npath = new_path;
      if (! exists(npath)) {
	SdFile::dateTimeCallback(SDCardDateTime);
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


void SDCard::latestDir(const char *path,
		       char *folder, size_t nfolder) {
  folder[0] = '\0';
  if (!checkAvailability())
    return;
  
  SdFile file;
  FsFile dir = sdfs.open(path);
  if (!dir)
    return;
  uint16_t ldate = 0;
  uint16_t ltime = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (! file.isDir())
      continue;
    uint16_t fdate;
    uint16_t ftime;
    file.getCreateDateTime(&fdate, &ftime);
    if ((fdate > ldate) || ((fdate == ldate) && (ftime > ltime))) {
      file.getName(folder, nfolder);
      ldate = fdate;
      ltime = ftime;
    }
  }
}


bool remove_file(SDClass &sdc, const char *path, const char *fname,
		 bool remove, Stream &stream) {
  char old_name[128];
  strcpy(old_name, path);
  strcat(old_name, "/");
  strcat(old_name, fname);
  if (remove) {
    bool r = sdc.sdfs.remove(old_name);
    if (r)
      stream.printf("  erased \"%s/%s\"\n", path, fname);
    else
      stream.printf("  failed to erase \"%s/%s\"\n",
		    path, fname);
    return r;
  }
  else {
    char new_name[128];
    strcpy(new_name, path);
    strcat(new_name, "/trash/");
    strcat(new_name, fname);
    bool r = sdc.sdfs.rename(old_name, new_name);
    if (r)
      stream.printf("  moved \"%s\" to \"%s\".\n",
		    fname, new_name);
    else
      stream.printf("  failed to move \"%s\" to \"%s\".\n",
		    fname, new_name);
    return r;
  }
}


int SDCard::cleanDir(const char *path, uint64_t min_size, const char *suffix,
		     bool associated, bool remove, Stream &stream) {
  if (!checkAvailability(stream))
    return 0;
  
  SdFile file;
  FsFile dir = sdfs.open(path);
  if (!dir) {
    stream.printf("Folder \"%s\" does not exist on %s SD card.\n", path, Name);
    return 0;
  }
  stream.printf("Clean directory on %sSD card:\n", Name);
  if (remove)
    stream.printf("Erase small files in \"%s\"/:\n", path);
  else
    stream.printf("Move small files in \"%s/\" to trash/:\n", path);
  // 1. number of small and large files:
  int n_small = 0;
  int n_large = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    char fname[64];
    file.getName(fname, 64);
    if (!file.isDir() && file.fileSize() < min_size &&
	strcmp(fname + max(0U, strlen(fname) - strlen(suffix)), suffix) == 0)
      n_small++;
    else
      n_large++;
  }
  dir.close();
  int n_removed = 0;
  if (n_small == 0)
    stream.print("  no small files found.\n");
  else {
    // 2. names of small files:
    char file_names[n_small][64];
    int k = 0;
    dir = sdfs.open(path);
    while (file.openNext(&dir, O_RDONLY)) {
      char fname[64];
      file.getName(fname, 64);
      if (!file.isDir() && file.fileSize() < min_size &&
	  strcmp(fname + max(0U, strlen(fname) - strlen(suffix)), suffix) == 0)
	file.getName(file_names[k++], 64);
    }
    dir.close();
    // 3. make trash directory:
    if (!remove) {
      dir = sdfs.open(path);
      FsFile trash = sdfs.open(path);
      if (dir.mkdir(&trash, "trash"))
	stream.printf("  created directory \"%s/trash/\"\n", path);
      else
	stream.printf("  failed to create directory \"%s/trash/\"\n", path);
      trash.close();
      dir.close();
    }
    // 4. move/remove small files:
    dir = sdfs.open(path);
    for (k=0; k<n_small; k++) {
      if (remove_file(*this, path, file_names[k], remove, stream))
	n_removed++;
      else
	n_large++;
    }
    dir.close();
    if (associated) {
      // 5. number of associated files:
      stream.println("Check for associated files");
      dir = sdfs.open(path);
      int n_assoc = 0;
      while (file.openNext(&dir, O_RDONLY)) {
	char fname[64];
	file.getName(fname, 64);
	for (int k=0; k<n_small; k++) {
	  char sname[64];
	  strcpy(sname, file_names[k]);
	  sname[max(0U, strlen(sname) - strlen(suffix))] = '\0';
	  if (strncmp(fname, sname, strlen(sname)) == 0)
	    n_assoc++;
	}
      }
      if (n_assoc == 0)
	stream.print("  no associated files found.\n");
      else {
	// 6. names of associated files:
	char more_file_names[n_assoc][64];
	int j = 0;
	dir = sdfs.open(path);
	while (file.openNext(&dir, O_RDONLY)) {
	  char fname[64];
	  file.getName(fname, 64);
	  for (int k=0; k<n_small; k++) {
	    char sname[64];
	    strcpy(sname, file_names[k]);
	    sname[max(0U, strlen(sname) - strlen(suffix))] = '\0';
	    if (strncmp(fname, sname, strlen(sname)) == 0)
	      file.getName(more_file_names[j++], 64);
	  }
	}
	dir.close();
	// 7. move/remove small files:
	dir = sdfs.open(path);
	for (k=0; k<n_assoc; k++) {
	  if (remove_file(*this, path, more_file_names[k],
			  remove, stream)) {
	    n_removed++;
	    n_large--;
	  }
	}
	dir.close();
      }
    }
  }
  // 6. move/remove empty directory:
  if (n_large == 0) {
    if (remove) {
      if (sdfs.rmdir(path)) {
	stream.printf("removed directory \"%s/\".\n", path);
	n_removed++;
      }
      else
	stream.printf("failed to remove directory \"%s/\".\n", path);
    }
    else {
      if (sdfs.mkdir("trash"))
	stream.print("created directory \"/trash/\"\n");
      char new_name[128];
      strcat(new_name, "trash/");
      strcat(new_name, path);
      if (sdfs.rename(path, new_name)) {
	stream.printf("moved directory \"%s\" to \"/trash/\".\n",
		      path);
	n_removed++;
      }
      else
	stream.printf("failed to move directory \"%s\" to \"/trash/\".\n",
		      path);
    }
  }
  return n_removed;
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
  if (list_sizes)
    stream.print("  size (bytes)  name\n");
  float file_sizes = 0.0;
  int n_files = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    char fname[64];
    file.getName(fname, 64);
    if (!file.isDir() || list_dirs) {
      stream.print("  ");
      if (list_sizes) {
        stream.printf("%12lu  ", file.fileSize());
        file_sizes += 1e-6*file.fileSize();
      }
      if (file.isDir())
	stream.printf("%s/\n", fname);
      else
	stream.printf("%s\n", fname);
      n_files++;
    }
  }
  if (n_files == 0)
    stream.print("\nno files found.\n");
  else {
    if (n_files > 1)
      stream.printf("%d files", n_files);
    else
      stream.printf("%d file", n_files);
    if (list_sizes)
      stream.printf(" (%.3f MB).\n", file_sizes);
    else
      stream.print(".\n");
  }
}


void SDCard::listDirectories(const char *path, bool list_dirs, bool list_sizes,
			     Stream &stream) {
  if (!checkAvailability(stream))
    return;
  
  FsFile dir = sdfs.open(path);
  if (!dir) {
    stream.printf("Folder \"%s\" does not exist on %s SD card.\n", path, Name);
    return;
  }
  stream.printf("Files on %sSD card:\n", Name);
  if (list_sizes)
    stream.print("  size (bytes)  name\n");
  char latest_folder[128];
  latestDir(path, latest_folder, 128);
  int n_dirs = 0;
  SdFile subdir;
  while (subdir.openNext(&dir, O_RDONLY)) {
    if (!subdir.isDir())
      continue;
    char subdir_name[64];
    subdir.getName(subdir_name, 64);
    if (strcmp(subdir_name, latest_folder) == 0)
      stream.printf("Files in \"%s\" (newest):\n", subdir_name);
    else
      stream.printf("Files in \"%s\":\n", subdir_name);
    float file_sizes = 0.0;
    int n_files = 0;
    SdFile file;
    while (file.openNext(&subdir, O_RDONLY)) {
      char fname[64];
      file.getName(fname, 64);
      if (!file.isDir() || list_dirs) {
	stream.print("  ");
	if (list_sizes) {
	  stream.printf("%12lu  ", file.fileSize());
	  file_sizes += 1e-6*file.fileSize();
	}
	if (file.isDir())
	  stream.printf("%s/\n", fname);
	else
	  stream.printf("%s\n", fname);
	n_files++;
      }
    }
    if (n_files == 0)
      stream.print("no files.\n");
    else {
      if (n_files > 1)
	stream.printf("%d files", n_files);
      else
	stream.printf("%d file", n_files);
      if (list_sizes)
	stream.printf(" (%.3f MB).\n", file_sizes);
      else
	stream.print(".\n");
    }
    stream.print("\n");
    n_dirs++;
  }
  if (n_dirs == 0)
    stream.print("\nno directories found.\n");
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
  stream.printf("Erase files on %sSD card:\n", Name);
  stream.printf("Erase all files in \"%s\"/:\n", path);
  int n = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isDir()) {
      char fname[64];
      file.getName(fname, 64);
      if (dir.remove(fname)) {
	stream.printf("  erased %s\n", fname);
	n++;
      }
      else
	stream.printf("  failed to erase file %s\n", fname);
    }
  }
  if (n == 0)
    stream.print("no files found.\n");
  else if (n == 1)
    stream.printf("Removed %d file.\n", n);
  else
    stream.printf("Removed %d files.\n", n);
  if (dir.rmdir())
    stream.printf("Removed folder \"%s\".\n", path);
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


void SDCard::report(Stream &stream, size_t indent,
		    size_t indent_delta) {
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
  
  stream.printf("%*s%sSD card:\n", indent, "", Name);
  indent += indent_delta;
  stream.printf("%*sManufacturer ID:    %x\n", indent, "", cid.mid);
  stream.printf("%*sOEM ID:             %c%c\n", indent, "",
		cid.oid[0], cid.oid[1]);
  stream.printf("%*sProduct:            %c%c%c%c%c\n", indent, "",
		cid.pnm[0], cid.pnm[1], cid.pnm[2], cid.pnm[3], cid.pnm[4]);
  stream.printf("%*sVersion:            %d.%d\n", indent, "",
		cid.prv_n, cid.prv_m);
  stream.printf("%*sSerial number:      %lx\n", indent, "", cid.psn);
  stream.printf("%*sManufacturing date: %d/%d\n", indent, "",
		cid.mdt_month, 2000 + cid.mdt_year_low + 10*cid.mdt_year_high);
  stream.printf("%*sType:               %s\n", indent, "", types);
  stream.printf("%*sFile system:        %s\n", indent, "", files);
  stream.printf("%*sCapacity:           %.3f GB\n", indent, "",
		1e-9 * cap);
  stream.printf("%*sAvailable:          %.3f GB\n", indent, "",
		1e-9 * free());
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


String SDCard::incrementFileName(const String &fname, Stream &stream) {
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
  if (num &&
      numinx + 4 < (int)fname.length() &&
      isdigit(fname[numinx + 3])) {
      width = fname[numinx + 3] - '0';
      nums[3] = fname[numinx + 3];
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
	  stream.printf("WARNING: file name overflow on %sSD card for \"%s\".\n",
			Name, fname);
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
	if (NameCounter > maxn) {
	  stream.printf("WARNING: file name overflow on %sSD card for \"%s\".\n",
			Name, fname);
	  return "";
	}
	volatile int nn_size = sizeof(nn); // avoid truncation warning: https://stackoverflow.com/a/70938456
	snprintf(nn, nn_size, "%0*d", width, NameCounter);
	nn[11] = '\0';
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
  SdFile::dateTimeCallback(SDCardDateTime);
  return sdfs.open(path, O_READ);
}


FsFile SDCard::openWrite(const char *path) {
  SdFile::dateTimeCallback(SDCardDateTime);
  return sdfs.open(path, O_WRONLY | O_CREAT | O_TRUNC );
}


FsFile SDCard::openAppend(const char *path) {
  SdFile::dateTimeCallback(SDCardDateTime);
  return sdfs.open(path, O_WRONLY | O_CREAT | O_AT_END);
}

