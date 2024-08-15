/*
  FirmwareUpdate - Upload hex file from SD card
  Created by Jan Benda, August 15th, 2024.
*/

#ifndef FirmwareUpdate_h
#define FirmwareUpdate_h

//#define FIRMWARE_UPDATE 1

#ifdef FIRMWARE_UPDATE


class SDCard;


void listFirmware(SDCard &sdcard, Stream &stream=Serial);

void updateFirmware(SDCard &sdcard, Stream &stream=Serial);


#endif

#endif

