/*
  PowerSave - Functions for reducing power consumption.
  Created by Jan Benda, November 27th, 2024.
*/

#ifndef PowerSave_h
#define PowerSave_h

/* Shutdown, i.e. power off Teensy. Need to reconnect power to reboot.
   Available for Teensy 4.x only. */
void shutdown_teensy();

/* Shutdown USB driver. */
void shutdown_usb();

/* Shutdown I/O pins. */
void shutdown_pins();

#endif
