#include "TeensyBoard.h"


const char *teensyStr() {
#if defined(TEENSY41)
  return (const char *)"Teensy 4.1";
#elif defined(TEENSY40)
  return (const char *)"Teensy 4.0";
#elif defined(TEENSY36)
  return (const char *)"Teensy 3.6";
#elif defined(TEENSY35)
  return (const char *)"Teensy 3.5";
#elif defined(TEENSY31)
  return (const char *)"Teensy 3.1";
#elif defined(TEENSY30)
  return (const char *)"Teensy 3.0";
#elif defined(TEENSYLC)
  return (const char *)"Teensy LC";
#else
  return (const char *)"unknown";
#endif
}

