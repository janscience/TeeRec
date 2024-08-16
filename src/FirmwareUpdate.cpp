#include <Arduino.h>
#include <Action.h>
#include <SDWriter.h>
#include <FirmwareUpdate.h>


#ifdef FIRMWARE_UPDATE


#include "FXUtil.h"		// read_ascii_line(), hex file support
extern "C" {
  #include "FlashTxx.h"		// TLC/T3x/T4x/TMM flash primitives
}


int listFirmware(SDCard &sdcard, Stream &stream, bool number) {
  SdFile file;
  SdFile dir;
  sdcard.rootDir();
  if (!dir.open("/")) {
    stream.printf("! ERROR: Faild to access root folder on SD card.\n");
    return 0;
  }
  stream.println("Available firmware files on SD card:");
  int n = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isDir()) {
      char fname[200];
      file.getName(fname, 200);
      if (strlen(fname) > 4 && strcmp(fname + strlen(fname) - 4, ".hex") == 0) {
	if (number)
	  stream.printf("- %d) ", n+1);
	else
	  stream.print("  ");
	stream.println(fname);
	n++;
      }
    }
  }
  if (n == 0)
    stream.printf("No firmware files found.\n");
  return n;
}


void updateFirmware(SDCard &sdcard, Stream &stream) {
  // list firmware files:
  int n = listFirmware(sdcard, stream, true);
  stream.println();
  if (n == 0)
    return;
  // select firmware file:
  int m = 0;
  if (n > 1) {
    while (true) {
      stream.print("Select a firmfile file [1]: ");
      while (stream.available() == 0) {
	yield();
      }
      char pval[32];
      stream.readBytesUntil('\n', pval, 32);
      if (strlen(pval) == 0)
	strcpy(pval, "1");
      stream.println(pval);
      char *end;
      long i = strtol(pval, &end, 10) - 1;
      if (end != pval && i >= 0 && i < (long)n) {
	m = i;
	stream.println();
	break;
      }
      else if (strcmp(pval, "q") == 0) {
	stream.println();
	stream.println("Firmware update aborted.");
	return;
      }
    }
  }
  // get firmware file:
  SdFile file;
  SdFile dir;
  sdcard.rootDir();
  if (!dir.open("/")) {
    stream.printf("! ERROR: Faild to access root folder on SD card.\n");
    stream.println("Firmware update aborted.");
    return;
  }
  n = 0;
  char hex_file_name[200];
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isDir()) {
      file.getName(hex_file_name, 200);
      if (strlen(hex_file_name) > 4 &&
	  strcmp(hex_file_name + strlen(hex_file_name) - 4, ".hex") == 0) {
	if (n == m)
	  break;
	n++;
      }
    }
  }
  stream.printf("Selected \"%s\" for firmware update\n", hex_file_name);
  stream.println();
  // check again:
  stream.println("WARNING: a firmware update could make your device unusable!");
  stream.println("WARNING: make sure that your device stays powered during the entire firmware update!");
  stream.println();
  if (!Action::yesno("Do you really want to update the firmware?",
		     false, stream)) {
    stream.println();
    stream.println("Firmware update aborted.");
    return;
  }
  stream.println();
  // open firmware file:
  File hex_file = sdcard.open(hex_file_name, FILE_READ);
  if (!hex_file) {
    stream.printf("! ERROR: Failed to open firmware file \"%s\" on SD card.\n",
		  hex_file_name);
    stream.println();
    stream.printf("Firmware update aborted.");
    return;
  }
  stream.printf("Successfully opened firmware file \"%s\".\n",
		hex_file_name);
  // create flash buffer to hold new firmware:
  stream.println();
  stream.println("Updating firmware:");
  stream.println("- initializing flash buffer ...");
  uint32_t buffer_addr, buffer_size;
  if (firmware_buffer_init( &buffer_addr, &buffer_size ) == 0) {
    stream.printf("! ERROR: Failed to create flash buffer.\n");
    stream.println();
    stream.println("! REBOOT SYSTEM !");
    stream.println();
    stream.flush();
    REBOOT;
  }
  stream.printf("- created flash buffer = %1luK %s (%08lX - %08lX)\n",
		buffer_size/1024, IN_FLASH(buffer_addr) ? "FLASH" : "RAM",
		buffer_addr, buffer_addr + buffer_size);
  // read hex file, write new firmware to flash, clean up, reboot
  stream.println("- updating frimware ...");
  stream.println();
  update_firmware(&hex_file, &stream, buffer_addr, buffer_size);
  // return from update_firmware() means error or user abort, so clean up and
  // reboot to ensure that static vars get boot-up initialized before retry
  stream.println();
  stream.printf("! ERROR: Failed to update firmware.\n");
  stream.println();
  stream.printf("Erase flash buffer / free RAM buffer...\n");
  stream.println();
  stream.println("! REBOOT SYSTEM !");
  stream.flush();
  firmware_buffer_free( buffer_addr, buffer_size );
  REBOOT;
}


#endif

