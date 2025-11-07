#include <TeensyBoard.h>
#include <Device.h>
#include <SDCard.h>
#include <DeviceID.h>
#include <DiagnosticMenu.h>


#ifdef TEENSY41
extern "C" uint8_t external_psram_size;
#endif


TeensyInfoAction::TeensyInfoAction(Menu &menu, const char *name) :
  InfoAction(menu, name, StreamIO | Report) {
  add("Board", teensyBoard());
  add("CPU speed", teensySpeedStr());
  add("Serial number", teensySN());
  add("MAC address", teensyMAC());
}


void TeensyInfoAction::update() {
  setValue("CPU speed", teensySpeedStr());
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


void PSRAMTestAction::execute(Stream &instream, Stream &outstream,
			      unsigned long timeout, bool echo,
			      bool detailed) {
#ifdef TEENSY41
  // from https://github.com/PaulStoffregen/teensy41_psram_memtest/blob/master/teensy41_psram_memtest.ino:
  uint8_t size = external_psram_size;
  outstream.println("EXTMEM Memory Test:");
  if (size == 0) {
    outstream.println("  no external PSRAM memory installed.\n");
    return;
  }
  long orig_speed = teensySpeed();
  setTeensySpeed(600);
  MemoryBegin = (uint32_t *)(0x70000000);
  MemoryEnd = (uint32_t *)(0x70000000 + size * 1048576);
  elapsedMillis msec = 0;
  if (!checkFixed(0x5A698421, outstream)) return;
  if (!checkRandom(2976674124ul, outstream)) return;
  if (!checkRandom(1438200953ul, outstream)) return;
  if (!checkRandom(3413783263ul, outstream)) return;
  if (!checkRandom(1900517911ul, outstream)) return;
  if (!checkRandom(1227909400ul, outstream)) return;
  if (!checkRandom(276562754ul, outstream)) return;
  if (!checkRandom(146878114ul, outstream)) return;
  if (!checkRandom(615545407ul, outstream)) return;
  if (!checkRandom(110497896ul, outstream)) return;
  if (!checkRandom(74539250ul, outstream)) return;
  if (!checkRandom(4197336575ul, outstream)) return;
  if (!checkRandom(2280382233ul, outstream)) return;
  if (!checkRandom(542894183ul, outstream)) return;
  if (!checkRandom(3978544245ul, outstream)) return;
  if (!checkRandom(2315909796ul, outstream)) return;
  if (!checkRandom(3736286001ul, outstream)) return;
  if (!checkRandom(2876690683ul, outstream)) return;
  if (!checkRandom(215559886ul, outstream)) return;
  if (!checkRandom(539179291ul, outstream)) return;
  if (!checkRandom(537678650ul, outstream)) return;
  if (!checkRandom(4001405270ul, outstream)) return;
  if (!checkRandom(2169216599ul, outstream)) return;
  if (!checkRandom(4036891097ul, outstream)) return;
  if (!checkRandom(1535452389ul, outstream)) return;
  if (!checkRandom(2959727213ul, outstream)) return;
  if (!checkRandom(4219363395ul, outstream)) return;
  if (!checkRandom(1036929753ul, outstream)) return;
  if (!checkRandom(2125248865ul, outstream)) return;
  if (!checkRandom(3177905864ul, outstream)) return;
  if (!checkRandom(2399307098ul, outstream)) return;
  if (!checkRandom(3847634607ul, outstream)) return;
  if (!checkRandom(27467969ul, outstream)) return;
  if (!checkRandom(520563506ul, outstream)) return;
  if (!checkRandom(381313790ul, outstream)) return;
  if (!checkRandom(4174769276ul, outstream)) return;
  if (!checkRandom(3932189449ul, outstream)) return;
  if (!checkRandom(4079717394ul, outstream)) return;
  if (!checkRandom(868357076ul, outstream)) return;
  if (!checkRandom(2474062993ul, outstream)) return;
  if (!checkRandom(1502682190ul, outstream)) return;
  if (!checkRandom(2471230478ul, outstream)) return;
  if (!checkRandom(85016565ul, outstream)) return;
  if (!checkRandom(1427530695ul, outstream)) return;
  if (!checkRandom(1100533073ul, outstream)) return;
  if (!checkFixed(0x55555555, outstream)) return;
  if (!checkFixed(0x33333333, outstream)) return;
  if (!checkFixed(0x0F0F0F0F, outstream)) return;
  if (!checkFixed(0x00FF00FF, outstream)) return;
  if (!checkFixed(0x0000FFFF, outstream)) return;
  if (!checkFixed(0xAAAAAAAA, outstream)) return;
  if (!checkFixed(0xCCCCCCCC, outstream)) return;
  if (!checkFixed(0xF0F0F0F0, outstream)) return;
  if (!checkFixed(0xFF00FF00, outstream)) return;
  if (!checkFixed(0xFFFF0000, outstream)) return;
  if (!checkFixed(0xFFFFFFFF, outstream)) return;
  if (!checkFixed(0x00000000, outstream)) return;
  setTeensySpeed(orig_speed);
  outstream.printf("test ran for %.2f seconds\n", (float)msec / 1000.0f);
  outstream.println("All memory tests passed :-)"); 
  outstream.println();
#else
  outstream.printf("%s does not support external PSRAM memory\n\n", teensyBoard());
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


void DevicesAction::execute(Stream &instream, Stream &outstream,
			    unsigned long timeout, bool echo,
			    bool detailed) {
  size_t navailable = 0;
  for (size_t k=0; k<NDevices; k++) {
    if (Devices[k]->available())
      navailable++;
  }
  // report:
  char ds[2] = {'\0', '\0'};
  if (NDevices != 1)
    ds[0] = 's';
  outstream.printf("%d of %d device%s available:\n",
		   navailable, NDevices, ds);
  for (size_t k=0; k<NDevices; k++) {
    if (Devices[k]->available()) {
      outstream.print("  ");
      Devices[k]->Device::report(outstream);
    }
  }
  if (navailable == 0)
    outstream.println("  no device available!");
  outstream.println();
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
    DevID->report(stream, indent, indentation());
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
