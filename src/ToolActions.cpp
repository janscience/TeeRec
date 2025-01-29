#include <TeensyBoard.h>
#include <RTClock.h>
#include <SDWriter.h>
#include <Configurator.h>
#include <Settings.h>
#include <Device.h>
#include <Input.h>
#include <InputSettings.h>
#include <ToolActions.h>


HelpAction::HelpAction(const char *name) :
  HelpAction(*Configurator::MainConfig->Config, name) {
}


HelpAction::HelpAction(Configurable &menu, const char *name) :
  Action(menu, name, StreamInput) {
}


void HelpAction::configure(Stream &stream, unsigned long timeout,
			   bool echo, bool detailed) {
  stream.println("Select menu entries by entering the number followed by 'return'.");
  stream.println("Go up to the parent menu by entering 'q'.");
  stream.println();
}


TeensyInfoAction::TeensyInfoAction(const char *name) :
  TeensyInfoAction(*Configurator::MainConfig->Config, name) {
}


TeensyInfoAction::TeensyInfoAction(Configurable &menu, const char *name) :
  Action(menu, name, StreamInput) {
}


void TeensyInfoAction::configure(Stream &stream, unsigned long timeout,
				 bool echo, bool detailed) {
  stream.println("Teensy development board:");
  stream.printf("  board        : %s\n", teensyBoard());
  stream.printf("  CPU speed    : %ldMHz\n", teensySpeed());
  stream.printf("  serial number: %s\n", teensySN());
  stream.printf("  MAC address  : %s\n", teensyMAC());
  stream.println();
}


#ifdef TEENSY41
extern "C" uint8_t external_psram_size;
#endif

void PSRAMInfoAction::configure(Stream &stream, unsigned long timeout,
				bool echo, bool detailed) {
#ifdef TEENSY41
  // from https://github.com/PaulStoffregen/teensy41_psram_memtest/blob/master/teensy41_psram_memtest.ino:
  stream.println("EXTMEM PSRAM Memory:");
  uint8_t size = external_psram_size;
  if (size == 0) {
    stream.println("  no external PSRAM memory installed.\n");
    return;
  }
  const float clocks[4] = {396.0f, 720.0f, 664.62f, 528.0f};
  const float frequency = clocks[(CCM_CBCMR >> 8) & 3] / (float)(((CCM_CBCMR >> 29) & 7) + 1);
  stream.printf("  size     : %u MB\n", size);
  stream.printf("  CCM_CBCMR: %08X (%.1f MHz)\n", CCM_CBCMR, frequency);
  stream.println();
#else
  stream.printf("%s does not support external PSRAM memory\n\n", teensyBoard());
#endif
}


bool PSRAMTestAction::checkFixed(uint32_t pattern, Stream &stream) {
  // fill the entire RAM with a fixed pattern, then check it:
  volatile uint32_t *p;
  stream.printf("  testing with fixed pattern %08X\n", pattern);
  for (p = MemoryBegin; p < MemoryEnd; p++)
    *p = pattern;
  arm_dcache_flush_delete((void *)MemoryBegin,
			  (uint32_t)MemoryEnd - (uint32_t)MemoryBegin);
  for (p = MemoryBegin; p < MemoryEnd; p++) {
    uint32_t actual = *p;
    if (actual != pattern) {
      stream.printf("  ERROR at %08X, read %08X but expected %08X\n",
		    (uint32_t)p, actual, pattern);
      return false;
    }
  }
  return true;
}


bool PSRAMTestAction::checkRandom(uint32_t seed, Stream &stream) {
  // fill the entire RAM with a pseudo-random sequence, then check it:
  volatile uint32_t *p;
  uint32_t reg;

  stream.printf("  testing with pseudo-random sequence, seed=%u\n", seed);
  reg = seed;
  for (p = MemoryBegin; p < MemoryEnd; p++) {
    *p = reg;
    for (int i=0; i < 3; i++) {
      // https://en.wikipedia.org/wiki/Xorshift
      reg ^= reg << 13;
      reg ^= reg >> 17;
      reg ^= reg << 5;
    }
  }
  arm_dcache_flush_delete((void *)MemoryBegin,
			  (uint32_t)MemoryEnd - (uint32_t)MemoryBegin);
  reg = seed;
  for (p = MemoryBegin; p < MemoryEnd; p++) {
    uint32_t actual = *p;
    if (actual != reg) {
      stream.printf("  ERROR at %08X, read %08X but expected %08X\n",
		    (uint32_t)p, actual, reg);
      return false;
    }
    for (int i=0; i < 3; i++) {
      reg ^= reg << 13;
      reg ^= reg >> 17;
      reg ^= reg << 5;
    }
  }
  return true;
}


void PSRAMTestAction::configure(Stream &stream, unsigned long timeout,
				bool echo, bool detailed) {
#ifdef TEENSY41
  // from https://github.com/PaulStoffregen/teensy41_psram_memtest/blob/master/teensy41_psram_memtest.ino:
  uint8_t size = external_psram_size;
  stream.println("EXTMEM Memory Test:");
  if (size == 0) {
    stream.println("  no external PSRAM memory installed.\n");
    return;
  }
  long orig_speed = teensySpeed();
  setTeensySpeed(600);
  MemoryBegin = (uint32_t *)(0x70000000);
  MemoryEnd = (uint32_t *)(0x70000000 + size * 1048576);
  elapsedMillis msec = 0;
  if (!checkFixed(0x5A698421, stream)) return;
  if (!checkRandom(2976674124ul, stream)) return;
  if (!checkRandom(1438200953ul, stream)) return;
  if (!checkRandom(3413783263ul, stream)) return;
  if (!checkRandom(1900517911ul, stream)) return;
  if (!checkRandom(1227909400ul, stream)) return;
  if (!checkRandom(276562754ul, stream)) return;
  if (!checkRandom(146878114ul, stream)) return;
  if (!checkRandom(615545407ul, stream)) return;
  if (!checkRandom(110497896ul, stream)) return;
  if (!checkRandom(74539250ul, stream)) return;
  if (!checkRandom(4197336575ul, stream)) return;
  if (!checkRandom(2280382233ul, stream)) return;
  if (!checkRandom(542894183ul, stream)) return;
  if (!checkRandom(3978544245ul, stream)) return;
  if (!checkRandom(2315909796ul, stream)) return;
  if (!checkRandom(3736286001ul, stream)) return;
  if (!checkRandom(2876690683ul, stream)) return;
  if (!checkRandom(215559886ul, stream)) return;
  if (!checkRandom(539179291ul, stream)) return;
  if (!checkRandom(537678650ul, stream)) return;
  if (!checkRandom(4001405270ul, stream)) return;
  if (!checkRandom(2169216599ul, stream)) return;
  if (!checkRandom(4036891097ul, stream)) return;
  if (!checkRandom(1535452389ul, stream)) return;
  if (!checkRandom(2959727213ul, stream)) return;
  if (!checkRandom(4219363395ul, stream)) return;
  if (!checkRandom(1036929753ul, stream)) return;
  if (!checkRandom(2125248865ul, stream)) return;
  if (!checkRandom(3177905864ul, stream)) return;
  if (!checkRandom(2399307098ul, stream)) return;
  if (!checkRandom(3847634607ul, stream)) return;
  if (!checkRandom(27467969ul, stream)) return;
  if (!checkRandom(520563506ul, stream)) return;
  if (!checkRandom(381313790ul, stream)) return;
  if (!checkRandom(4174769276ul, stream)) return;
  if (!checkRandom(3932189449ul, stream)) return;
  if (!checkRandom(4079717394ul, stream)) return;
  if (!checkRandom(868357076ul, stream)) return;
  if (!checkRandom(2474062993ul, stream)) return;
  if (!checkRandom(1502682190ul, stream)) return;
  if (!checkRandom(2471230478ul, stream)) return;
  if (!checkRandom(85016565ul, stream)) return;
  if (!checkRandom(1427530695ul, stream)) return;
  if (!checkRandom(1100533073ul, stream)) return;
  if (!checkFixed(0x55555555, stream)) return;
  if (!checkFixed(0x33333333, stream)) return;
  if (!checkFixed(0x0F0F0F0F, stream)) return;
  if (!checkFixed(0x00FF00FF, stream)) return;
  if (!checkFixed(0x0000FFFF, stream)) return;
  if (!checkFixed(0xAAAAAAAA, stream)) return;
  if (!checkFixed(0xCCCCCCCC, stream)) return;
  if (!checkFixed(0xF0F0F0F0, stream)) return;
  if (!checkFixed(0xFF00FF00, stream)) return;
  if (!checkFixed(0xFFFF0000, stream)) return;
  if (!checkFixed(0xFFFFFFFF, stream)) return;
  if (!checkFixed(0x00000000, stream)) return;
  setTeensySpeed(orig_speed);
  stream.printf("test ran for %.2f seconds\n", (float)msec / 1000.0f);
  stream.println("All memory tests passed :-)"); 
  stream.println();
#else
  stream.printf("%s does not support external PSRAM memory\n\n", teensyBoard());
#endif
}


ReportConfigAction::ReportConfigAction(const char *name) :
  ReportConfigAction(*Configurator::MainConfig->Config, name) {
}


ReportConfigAction::ReportConfigAction(Configurable &menu, const char *name) :
  Action(menu, name, StreamInput) {
}


void ReportConfigAction::configure(Stream &stream, unsigned long timeout,
				   bool echo, bool detailed) {
  root()->report(stream);
  stream.println();
}


SDCardAction::SDCardAction(const char *name, SDCard &sd) :
  SDCardAction(*Configurator::MainConfig->Config, name, sd) {
}


SDCardAction::SDCardAction(Configurable &menu, const char *name, SDCard &sd) : 
  Action(menu, name, StreamInput),
  SDC(sd) {
}


void SaveConfigAction::configure(Stream &stream, unsigned long timeout,
				 bool echo, bool detailed) {
  bool save = true;
  if (SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" already exists on SD card.\n",
		  root()->configFile());
    save = Action::yesno("Do you want to overwrite the configuration file?",
			 true, echo, stream);
  }
  if (save && root()->save(SDC))
    stream.printf("Saved configuration to file \"%s\" on SD card.\n",
		  root()->configFile());
  stream.println();
}


void LoadConfigAction::configure(Stream &stream, unsigned long timeout,
				 bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" not found on SD card.\n\n",
		  root()->configFile());
    return;
  }
  stream.println("Reloading the configuration file will discard all changes.");
  bool r = Action::yesno("Do you really want to reload the configuration file?",
			 true, echo, stream);
  stream.println();
  if (r)
    root()->load(SDC);
}


void RemoveConfigAction::configure(Stream &stream, unsigned long timeout,
				   bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.exists(root()->configFile())) {
    stream.printf("Configuration file \"%s\" does not exist on SD card.\n\n",
		  root()->configFile());
    return;
  }
  if (Action::yesno("Do you really want to remove the configuration file?",
		    false, echo, stream)) {
    if (SDC.remove(root()->configFile()))
      stream.printf("\nRemoved configuration file \"%s\" from SD card.\n\n",
		    root()->configFile());
    else
      stream.printf("\nERROR! Failed to remove configuration file \"%s\" from SD card.\n\n",
		    root()->configFile());
  }
  else
    stream.println();
}


void SDInfoAction::configure(Stream &stream, unsigned long timeout,
			     bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  SDC.report(stream);
}


void SDCheckAction::configure(Stream &stream, unsigned long timeout,
			      bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  SDC.check(1024*1024, stream);
}


void SDBenchmarkAction::configure(Stream &stream, unsigned long timeout,
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
      sprintf(request, "Should the configuration file \"%s\" be kept?",
	      root()->configFile());
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


void SDFormatAction::configure(Stream &stream, unsigned long timeout,
			       bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Formatting will destroy all data on the SD card.");
  format("", false, echo, stream);
}


void SDEraseFormatAction::configure(Stream &stream, unsigned long timeout,
				    bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  stream.println("Erasing and formatting will destroy all data on the SD card.");
  format(" erase and", true, echo, stream);
}


void SDListRootAction::configure(Stream &stream, unsigned long timeout,
				 bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listFiles("/", true, true, stream);
  stream.println();
}


SDListRecordingsAction::SDListRecordingsAction(const char *name, SDCard &sd,
					       Settings &settings) :
  SDCardAction(name, sd),
  SettingsMenu(settings) {
}


SDListRecordingsAction::SDListRecordingsAction(Configurable &menu,
					       const char *name,
					       SDCard &sd,
					       Settings &settings) : 
  SDCardAction(menu, name, sd),
  SettingsMenu(settings) {
}


void SDListRecordingsAction::configure(Stream &stream, unsigned long timeout,
				       bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  if (!SDC.checkAvailability(stream))
    return;
  SDC.listFiles(SettingsMenu.path(), false, true, stream);
  stream.println();
}


void SDRemoveRecordingsAction::configure(Stream &stream, unsigned long timeout,
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


void ListFirmwareAction::configure(Stream &stream, unsigned long timeout,
				   bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  listFirmware(SDC, stream);
  stream.println();
}


void UpdateFirmwareAction::configure(Stream &stream, unsigned long timeout,
				     bool echo, bool detailed) {
  if (disabled(StreamInput))
    return;
  updateFirmware(SDC, echo, stream);
  stream.println();
}


RTCAction::RTCAction(const char *name, RTClock &rtclock) :
  RTCAction(*root()->Config, name, rtclock) {
}


RTCAction::RTCAction(Configurable &menu, const char *name, RTClock &rtclock) :
  Action(menu, name, StreamInput),
  RTC(rtclock) {
}


void PrintRTCAction::configure(Stream &stream, unsigned long timeout,
			       bool echo, bool detailed) {
  stream.print("Current time: ");
  RTC.print(stream);
  stream.println();
}


void ReportRTCAction::configure(Stream &stream, unsigned long timeout,
				bool echo, bool detailed) {
  RTC.report(stream);
}


void SetRTCAction::configure(Stream &stream, unsigned long timeout,
			     bool echo, bool detailed) {
  RTC.set(stream);
  stream.println();
}


DevicesAction::DevicesAction(const char *name, Device* dev0, Device* dev1,
			     Device* dev2, Device* dev3,
			     Device* dev4, Device* dev5) :
  DevicesAction(*Configurator::MainConfig->Config, name,
		dev0, dev1, dev2, dev3, dev4, dev5) {
}


DevicesAction::DevicesAction(Configurable &menu, const char *name,
			     Device* dev0, Device* dev1,
			     Device* dev2, Device* dev3,
			     Device* dev4, Device* dev5) :
  Action(menu, name, StreamInput) {
  if (dev0 != 0)
    Devices[NDevices++] = dev0;
  if (dev1 != 0)
    Devices[NDevices++] = dev1;
  if (dev2 != 0)
    Devices[NDevices++] = dev2;
  if (dev3 != 0)
    Devices[NDevices++] = dev3;
  if (dev4 != 0)
    Devices[NDevices++] = dev4;
  if (dev5 != 0)
    Devices[NDevices++] = dev5;
}


void DevicesAction::configure(Stream &stream, unsigned long timeout,
			      bool echo, bool detailed) {
  size_t navailable = 0;
  for (size_t k=0; k<NDevices; k++) {
    if (Devices[k]->available())
      navailable++;
  }
  // report:
  char ds[2] = {'\0', '\0'};
  if (NDevices != 1)
    ds[0] = 's';
  stream.printf("%d of %d device%s available:\n",
                navailable, NDevices, ds);
  for (size_t k=0; k<NDevices; k++) {
    if (Devices[k]->available()) {
      stream.print("  ");
      Devices[k]->Device::report(stream);
    }
  }
  if (navailable == 0)
    stream.println("  no device available!");
  stream.println();
}


InputAction::InputAction(const char *name, Input &data,
			 InputSettings &settings,
			 Device** controls, size_t ncontrols,
			 SetupAI setupai) :
  InputAction(*root()->Config, name, data, settings,
	      controls, ncontrols, setupai) {
}


InputAction::InputAction(Configurable &menu, const char *name,
			 Input &data, InputSettings &settings,
			 Device** controls, size_t ncontrols,
			 SetupAI setupai) :
  Action(menu, name, StreamInput),
  Data(data),
  Settings(settings),
  Controls(controls),
  NControls(ncontrols),
  Setupai(setupai) {
}


void ReportInputAction::configure(Stream &stream, unsigned long timeout,
				  bool echo, bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, stream);
  if (!Data.check(0, stream)) {
    stream.println();
    return;
  }
  Data.begin();
  Data.start();
  Data.report(stream);
  Data.stop();
  Data.reset();
}


void PrintInputAction::configure(Stream &stream, unsigned long timeout,
				 bool echo, bool detailed) {
  Data.reset();
  Settings.configure(&Data);
  if (Setupai != 0)
    Setupai(Data, Settings, Controls, NControls, stream);
  if (!Data.check(0, stream)) {
    stream.println();
    return;
  }
  int tmax = 100;
  stream.print("Record some data ...");
  Data.begin();
  Data.start();
  delay(tmax);
  Data.stop();
  stream.println();
  size_t nframes = Data.index()/Data.nchannels();
  if (Data.frames(0.001*tmax) < nframes)
    nframes = Data.frames(0.001*tmax);
  stream.printf("Sampling rate: %dHz", Data.rate());
  stream.println();
  stream.printf("Resolution: %ubits", Data.dataResolution());
  stream.println();
  Data.printData(0, nframes, stream);
  stream.println();
}

