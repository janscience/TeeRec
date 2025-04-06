#include <Settings.h>
#include <SDCard.h>
#include <SDCardMenu.h>


SDCardAction::SDCardAction(Menu &menu, const char *name, SDCard &sd) : 
  Action(menu, name, StreamInput),
  SDC(sd) {
}


void SDInfoAction::execute(Stream &stream, unsigned long timeout,
			   bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  SDC.report(stream);
}


void SDCheckAction::execute(Stream &stream, unsigned long timeout,
			    bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  SDC.check(1024*1024, stream);
}


void SDBenchmarkAction::execute(Stream &stream, unsigned long timeout,
				bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  SDC.benchmark(512, 10, 2, stream);
}


void SDFormatAction::format(const char *erases, bool erase,
			    bool echo, Stream &stream) {
  char request[64];
  sprintf(request, "Do you really want to%s format the SD card?", erases);
  if (Action::yesno(request, false, echo, stream)) {
    bool keep = false;
    if (SDC.exists(root()->configFile())) {
      char request[256];
      snprintf(request, 256, "Should the configuration file \"%s\" be kept?",
	       root()->configFile());
      request[255] = '\0';
      keep = Action::yesno(request, true, echo, stream);
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


void SDFormatAction::execute(Stream &stream, unsigned long timeout,
			     bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Formatting will destroy all data on the SD card.");
  format("", false, echo, stream);
}


void SDEraseFormatAction::execute(Stream &stream, unsigned long timeout,
				  bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Erasing and formatting will destroy all data on the SD card.");
  format(" erase and", true, echo, stream);
}


void SDListRootAction::execute(Stream &stream, unsigned long timeout,
			       bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listFiles("/", true, true, stream);
  stream.println();
}


SDListRecordingsAction::SDListRecordingsAction(Menu &menu,
					       const char *name,
					       SDCard &sd,
					       Settings &settings) : 
  SDCardAction(menu, name, sd),
  SettingsMenu(settings) {
}


void SDListRecordingsAction::execute(Stream &stream, unsigned long timeout,
				     bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listFiles(SettingsMenu.path(), false, true, stream);
  stream.println();
}


void SDRemoveRecordingsAction::execute(Stream &stream, unsigned long timeout,
				       bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  if (! SDC.exists(SettingsMenu.path())) {
    stream.printf("Folder \"%s\" does not exist.\n\n", SettingsMenu.path());
    return;
  }
  stream.printf("Erase all files in folder \"%s\".\n", SettingsMenu.path());
  if (Action::yesno("Do you really want to erase all recordings?",
		    true, echo, stream))
    SDC.removeFiles(SettingsMenu.path(), stream);
  stream.println();
}


SDCardMenu::SDCardMenu(Menu &menu, SDCard &sdcard, Settings &settings) :
  Menu(menu, "SD card", Action::StreamInput),
  InfoAct(*this, "SD card info", sdcard),
  ListRootAct(*this, "List files in root directory", sdcard),
  ListRecsAct(*this, "List all recordings", sdcard, settings),
  EraseRecsAct(*this, "Erase all recordings", sdcard, settings),
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

