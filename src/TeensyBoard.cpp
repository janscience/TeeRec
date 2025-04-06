#include "TeensyBoard.h"


const char *teensyBoard() {
#if defined(TEENSY41)
  return (const char *)"Teensy 4.1";
#elif defined(TEENSY40)
  return (const char *)"Teensy 4.0";
#elif defined(TEENSY36)
  return (const char *)"Teensy 3.6";
#elif defined(TEENSY35)
  return (const char *)"Teensy 3.5";
#elif defined(TEENSY32)
  return (const char *)"Teensy 3.2";
#elif defined(TEENSY30)
  return (const char *)"Teensy 3.0";
#elif defined(TEENSYLC)
  return (const char *)"Teensy LC";
#else
  return (const char *)"unknown";
#endif
}


long teensySpeed() {
#if defined(__IMXRT1062__)
  return F_CPU_ACTUAL/1000000;
#else
  return F_CPU/1000000;
#endif
}


#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#endif

void setTeensySpeed(long speed) {
#if defined(__IMXRT1062__)
  set_arm_clock(1000000L*speed);
  if (teensySpeed() != speed)
    Serial.printf("WARNING: CPU speed is %ldMHz instead of the requested %ldMHz!\n", teensySpeed(), speed);
#endif
}


void reboot() {
#if defined(TEENSY4)
  SCB_AIRCR = 0x05FA0004;
#elif defined(TEENSY3)
  #define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
  *CPU_RESTART_ADDR = 0x5FA0004;
#endif
}


void halt(Stream &stream) {
  stream.println("HALT");
  char reboot_s[] = "reboot";
  size_t i = 0;
  while (true) {
    yield();
    int b = stream.read();
    if (b >= 0) {
      char c = char(b);
      if (c == reboot_s[i]) {
	i++;
	if (i >= strlen(reboot_s)) {
	  stream.println("REBOOT NOW");
	  delay(100);
	  reboot();
	}
      }
      else
	i = 0;
    }
  };
}


#if defined(TEENSY4)

static uint32_t getTeensySerial(void) {
  uint32_t num;
  num = HW_OCOTP_MAC0 & 0xFFFFFF;
  return num;
}

#else

static uint32_t getTeensySerial(void) {
  uint32_t num = 0;
  __disable_irq();
#if defined(HAS_KINETIS_FLASH_FTFA) || defined(HAS_KINETIS_FLASH_FTFL)
  FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
  FTFL_FCCOB0 = 0x41;
  FTFL_FCCOB1 = 15;
  FTFL_FSTAT = FTFL_FSTAT_CCIF;
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
  num = *(uint32_t *)&FTFL_FCCOB7;
#elif defined(HAS_KINETIS_FLASH_FTFE)
  kinetis_hsrun_disable();
  FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
  *(uint32_t *)&FTFL_FCCOB3 = 0x41070000;
  FTFL_FSTAT = FTFL_FSTAT_CCIF;
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
  num = *(uint32_t *)&FTFL_FCCOBB;
  kinetis_hsrun_enable();
#endif
  __enable_irq();
  return num;
}

#endif


void teensySN(uint8_t *sn) {
  uint32_t num = getTeensySerial();
  sn[0] = num >> 24;
  sn[1] = num >> 16;
  sn[2] = num >> 8;
  sn[3] = num;
}


const char* teensySN(void) {
  uint8_t serial[4];
  static char teensySerial[12];
  teensySN(serial);
  sprintf(teensySerial, "%02x-%02x-%02x-%02x", serial[0], serial[1], serial[2], serial[3]);
  return teensySerial;
}


#if defined(TEENSY4)

void teensyMAC(uint8_t *mac) { // there are 2 MAC addresses each 48bit 
  uint32_t m1 = HW_OCOTP_MAC1;
  uint32_t m2 = HW_OCOTP_MAC0;
  mac[0] = m1 >> 8;
  mac[1] = m1 >> 0;
  mac[2] = m2 >> 24;
  mac[3] = m2 >> 16;
  mac[4] = m2 >> 8;
  mac[5] = m2 >> 0;
}

#else

void teensyMAC(uint8_t *mac) {
  uint8_t serial[4];
  teensySN(serial);
  mac[0] = 0x04;
  mac[1] = 0xE9;
  mac[2] = 0xE5;
  mac[3] = serial[1];
  mac[4] = serial[2];
  mac[5] = serial[3];
}

#endif

const char *teensyMAC(void) {
  uint8_t mac[6];
  static char teensyMac[18];
  teensyMAC(mac);
  sprintf(teensyMac, "%02x:%02x:%02x:%02x:%02x:%02x",
	  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return teensyMac;
}


int analogPin(int8_t pin, char *pins) {
  for (int p=0; p<NAPins; p++) {
    if (APins[p] == pin) {
      int n = snprintf(pins, 4, "A%d", p);
      pins[3] = '\0';
      return n;
    }
  }
  pins[0] = '\0';
  return 0;
}

