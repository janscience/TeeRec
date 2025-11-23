#include <SDCard.h>
#include <SDCardMenu.h>


SDCardAction::SDCardAction(Menu &menu, const char *name, SDCard &sd,
			   unsigned int roles) : 
  Action(menu, name, roles),
  SDC(sd) {
}


SDInfoAction::SDInfoAction(Menu &menu, const char *name, SDCard &sd) : 
  SDCardAction(menu, name, sd, ReportRoles) {
}


void SDInfoAction::write(Stream &stream, unsigned int roles,
			 size_t indent, size_t width) const {
  SDC.write(stream, indent, indentation());
}


void SDCheckAction::execute(Stream &stream) {
  SDC.check(1024*1024, stream);
  if (gui()) {
    SDC.write(stream, 0, indentation());
    stream.println();
  }
}


void SDBenchmarkAction::execute(Stream &stream) {
  SDC.benchmark(512, 10, 2, stream);
}


void SDFormatAction::format(const char *erases, bool erase,
			    Stream &stream) {
  char request[64];
  sprintf(request, "Do you really want to%s format the SD card?", erases);
  if (Action::yesno(request, false, echo(), stream)) {
    bool keep = false;
    if (SDC.exists(root()->configFile())) {
      char request[256];
      snprintf(request, 256, "Should the configuration file \"%s\" be kept?",
	       root()->configFile());
      request[255] = '\0';
      keep = Action::yesno(request, true, echo(), stream);
    }
    stream.println();
    const char *path = NULL;
    if (keep)
      path = root()->configFile();
    SDC.format(path, erase, stream);
  }
  else
    stream.println();
}


void SDFormatAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Formatting will destroy all data on the SD card.");
  format("", false, stream);
}


void SDEraseFormatAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Erasing and formatting will destroy all data on the SD card.");
  format(" erase and", true, stream);
}


void SDListRootAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listFiles("/", true, true, stream);
  stream.println();
}


void SDListRecordingsAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listDirectories("/", true, true, stream);
  stream.println();
}


void SDRemoveRecordingsAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  char folder[64];
  SDC.latestDir("/", folder, 64);
  if (strlen(folder) == 0) {
    stream.print("No folder exists that can be removed.\n\n");
    return;
  }
  char msg[128];
  sprintf(msg, "Do you really want to erase all recordings in \"%s\"?",
	  folder);
  if (Action::yesno(msg, true, echo(), stream))
    SDC.removeFiles(folder, stream);
  stream.println();
}


SDCleanRecordingsAction::SDCleanRecordingsAction(Menu &menu,
						 const char *name,
						 SDCard &sd) :
  SDCardAction(menu, name, sd),
  MinSize(1024),
  Suffix(".wav"),
  Remove(false) {
}

void SDCleanRecordingsAction::setRemove(bool remove) {
  Remove = remove;
}

void SDCleanRecordingsAction::setMinSize(uint64_t min_size) {
  MinSize = min_size;
}

void SDCleanRecordingsAction::setSuffix(const char *suffix) {
  Suffix = suffix;
}

void SDCleanRecordingsAction::set(uint64_t min_size, const char *suffix, bool remove) {
  setMinSize(min_size);
  setSuffix(suffix);
  setRemove(remove);
}

void SDCleanRecordingsAction::execute(Stream &stream) {
  if (!SDC.checkAvailability(stream))
    return;
  char folder[64];
  SDC.latestDir("/", folder, 64);
  if (strlen(folder) == 0) {
    stream.print("No folder exists that can be cleaned.\n\n");
    return;
  }
  stream.printf("Clean up files in folder \"%s\".\n", folder);
  char msg[128];
  if (Remove)
    strcpy(msg, "Do you really want to erase small files in this folder?");
  else
    strcpy(msg, "Do you really want to move small files in this folder to trash/?");
  if (Action::yesno(msg, true, echo(), stream))
    SDC.cleanDir(folder, MinSize, Suffix, strlen(Suffix) > 0, Remove, stream);
  stream.println();
}


SDCardMenu::SDCardMenu(Menu &menu, SDCard &sdcard) :
  Menu(menu, "SD card", StreamInput),
  InfoAct(*this, "SD card info", sdcard),
  ListRootAct(*this, "List files in root directory", sdcard),
  ListRecsAct(*this, "List all recordings", sdcard),
  CleanRecsAct(*this, "Clean recent recordings", sdcard),
  EraseRecsAct(*this, "Erase recent recordings", sdcard),
  FormatAct(*this, "Format SD card", sdcard),
  EraseFormatAct(*this, "Erase and format SD card", sdcard) {
  if (strlen(sdcard.name()) > 0) {
    char name[strlen(sdcard.name()) + 8];
    strcpy(name, sdcard.name());
    name[0] = toupper(name[0]);
    strcat(name, "SD card");
    setName(name);
  }
}

