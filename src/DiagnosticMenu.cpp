#include <EEPROM.h>
#include <TeensyBoard.h>
#include <Device.h>
#include <SDCard.h>
#include <DeviceID.h>
#include <DiagnosticMenu.h>


#ifdef TEENSY41
extern "C" uint8_t external_psram_size;
#endif


TeensyInfoAction::TeensyInfoAction(Menu &menu, const char *name) :
  InfoAction(menu, name, StreamIO | Report),
  EEPROMLength("") {
  snprintf(EEPROMLength, 16, "%ubytes", EEPROM.length());
  add("Board", teensyBoard());
  add("CPU speed", teensySpeedStr());
  add("Serial number", teensySN());
  add("MAC address", teensyMAC());
  add("EEPROM size", EEPROMLength);
}


void TeensyInfoAction::update() {
  setValue("CPU speed", teensySpeedStr());
}


EEPROMHexdumpAction::EEPROMHexdumpAction(Menu &menu, const char *name) : 
  Action(menu, name, StreamIO) {
}


void EEPROMHexdumpAction::write(Stream &stream, unsigned int roles,
				size_t indent, size_t width,
				bool descend) const {
  unsigned int i=0;
  while (i < EEPROM.length()) {
    stream.printf("%04x  ", i);
    for (unsigned int j=0; j < 2 && i + 8*j < EEPROM.length(); j++) {
      for (unsigned int k=0; k < 8 && i + 8*j + k < EEPROM.length(); k++)
	stream.printf("%02x ", EEPROM[i + 8*j + k]);
      stream.print(" ");
    }
    stream.printf("|");
    for (unsigned int j=0; j < 16 && i < EEPROM.length(); j++)
      if (EEPROM[i] > 0x10 && EEPROM[i] < 0x80)
	stream.printf("%c", EEPROM[i++]);
      else
	stream.print('.');
    stream.print("|\n");
  }
  stream.println();
}


PSRAMInfoAction::PSRAMInfoAction(Menu &menu, const char *name) :
  InfoAction(menu, name, StreamIO | Report) {
  uint8_t size = 0;
  CCMStr[0] = '\0';
#ifdef TEENSY41
  // from https://github.com/PaulStoffregen/teensy41_psram_memtest/blob/master/teensy41_psram_memtest.ino:
  size = external_psram_size;
  if (size > 0) {
    const float clocks[4] = {396.0f, 720.0f, 664.62f, 528.0f};
    const float frequency = clocks[(CCM_CBCMR >> 8) & 3] / (float)(((CCM_CBCMR >> 29) & 7) + 1);
    sprintf(CCMStr, "%08lX (%.1f MHz)", CCM_CBCMR, frequency);
  }
#endif
  sprintf(SizeStr, "%u MB", size);
  add("Size", SizeStr);
  if (strlen(CCMStr) > 0)
    add("CCM_CBCMR", CCMStr);
}


PSRAMTestAction::PSRAMTestAction(Menu &menu, const char *name) : 
  Action(menu, name, StreamIO) {
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


void PSRAMTestAction::execute(Stream &stream) {
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


DevicesAction::DevicesAction(Menu &menu, const char *name,
			     Device* dev0, Device* dev1,
			     Device* dev2, Device* dev3,
			     Device* dev4, Device* dev5) :
  Action(menu, name, StreamIO | Report) {
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


void DevicesAction::write(Stream &stream, unsigned int roles,
			  size_t indent, size_t width, bool descend) const {
  if (disabled(roles))
    return;
  if (descend) {
    if (strlen(name()) > 0) {
      stream.printf("%*s%s:\n", indent, "", name());
      indent += indentation();
    }
    for (size_t k=0; k<NDevices; k++) {
      if (Devices[k]->available())
	Devices[k]->write(stream, indent, indentation());
    }
  }
  else if (strlen(name()) > 0)
    Action::write(stream, roles, indent, width, descend);
}


void DevicesAction::execute(Stream &stream) {
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


Device *DevicesAction::device(size_t index) {
  if (index >= NDevices)
    return 0;
  return Devices[index];
}


DeviceIDAction::DeviceIDAction(Menu &menu, const char *name,
			       DeviceID *deviceid) :
  Action(menu, name, StreamInput | Report),
  DevID(deviceid){
}


void DeviceIDAction::write(Stream &stream, unsigned int roles,
			   size_t indent, size_t width, bool descend) const {
  if (disabled(roles))
    return;
  if (descend) {
    DevID->read();
    DevID->write(stream, indent, indentation());
  }
  else
    Action::write(stream, roles, indent, width, descend);
}


DiagnosticMenu::DiagnosticMenu(Menu &menu, SDCard &sdcard,
			       DeviceID *devid,
			       Device* dev0, Device* dev1,
			       Device* dev2, Device* dev3,
			       Device* dev4, Device* dev5) :
  Menu(menu, "Diagnostics", Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  EEPROMHexdumpAct(*this, "EEPROM memory content"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0CheckAct(*this, "SDc", sdcard),
  SD0BenchmarkAct(*this, "SDb", sdcard),
  SD1CheckAct(*this, "SDc", sdcard),
  SD1BenchmarkAct(*this, "SDb", sdcard),
  DevicesAct(*this, "Input devices", dev0, dev1, dev2, dev3, dev4, dev5),
  DeviceIDAct(*this, "Device ID", devid) {
  setSDCardNames(sdcard, SD0CheckAct, SD0BenchmarkAct);
  SD1CheckAct.disable();
  SD1BenchmarkAct.disable();
  if (dev0 == 0)
    DevicesAct.disable();
  if (devid == 0)
    DeviceIDAct.disable();
}


DiagnosticMenu::DiagnosticMenu(Menu &menu, SDCard &sdcard0,
			       SDCard &sdcard1, DeviceID *devid,
			       Device* dev0, Device* dev1,
			       Device* dev2, Device* dev3,
			       Device* dev4, Device* dev5) :
  Menu(menu, "Diagnostics", Action::StreamInput),
  TeensyInfoAct(*this, "Teensy info"),
  EEPROMHexdumpAct(*this, "EEPROM memory content"),
  PSRAMInfoAct(*this, "PSRAM memory info"),
  PSRAMTestAct(*this, "PSRAM memory test"),
  SD0CheckAct(*this, "Primary SD card check", sdcard0),
  SD0BenchmarkAct(*this, "Primary SD card benchmark", sdcard0),
  SD1CheckAct(*this, "Secondary SD card check", sdcard1),
  SD1BenchmarkAct(*this, "Secondary SD card benchmark", sdcard1),
  DevicesAct(*this, "Input devices", dev0, dev1, dev2, dev3, dev4, dev5),
  DeviceIDAct(*this, "Device ID", devid) {
  setSDCardNames(sdcard0, SD0CheckAct, SD0BenchmarkAct);
  setSDCardNames(sdcard1, SD1CheckAct, SD1BenchmarkAct);
  if (dev0 == 0)
    DevicesAct.disable();
  if (devid == 0)
    DeviceIDAct.disable();
}


void DiagnosticMenu::updateCPUSpeed() {
  TeensyInfoAct.update();
}


void DiagnosticMenu::setSDCardNames(SDCard &sdcard, Action &checkact,
				    Action &benchmarkact) {
  char name[64];
  strcpy(name, sdcard.name());
  if (strlen(name) > 0)
    name[0] = toupper(name[0]);
  strcat(name, "SD card check");
  checkact.setName(name);
  name[strlen(name) - 5] = '\0';
  strcat(name, "benchmark");
  benchmarkact.setName(name);
}
