/*
  FirmwareUpdate - Upload hex file from SD card
  Created by Jan Benda, August 15th, 2024.
*/

#ifndef FirmwareUpdate_h
#define FirmwareUpdate_h

//#define FIRMWARE_UPDATE 1

#ifdef FIRMWARE_UPDATE


class SDCard;


// List hex files on SD card.
int listFirmware(SDCard &sdcard, Stream &stream=Serial, bool number=false);

// Update firmware from hex file on SD card.
// Uses https://github.com/joepasquariello/FlasherX
void updateFirmware(SDCard &sdcard, bool echo=true, Stream &stream=Serial);


#endif

#endif

