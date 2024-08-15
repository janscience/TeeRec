#include <Arduino.h>
#include <FirmwareUpdate.h>

#ifdef FIRMWARE_UPDATE


void listFirmware(SDCard &sdcard, Stream &stream) {
  stream.println("List firmware");
}


void updateFirmware(SDCard &sdcard, Stream &stream) {
  stream.println("Update firmware");
  // Put FlasherX code here
}


#endif

