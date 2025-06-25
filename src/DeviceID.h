/*
  DeviceID - read out bits from digital pins as a device identifier.
  Created by Jan Benda, September 16th, 2024.

  The idea is to give a device an identifying ID by manually setting
  DIP switches. Each switch represents a bit of a number. This way,
  the device can get an ID when it is assembled without the need of a
  computer to program an ID.  Then, at the start of a sketch, the
  DeviceID class can be used to read in this device ID. Use this ID
  for naming files, for example, via the makeStr() method.

  The switches are connected between Teensy pins and 3.3V. The Teensy
  pins are also connected via pull-down resistance to ground. In order
  to reduce power consumption, the 3.3V to the switches can be
  supplied from a dedicated digital pin. Then the switches (and
  pulldown resistances) are only powered when reading out the device
  ID.
*/

#ifndef DeviceID_h
#define DeviceID_h


#include <Arduino.h>


class DeviceID {

public:

  // Maximum number of pins (=bits) that can form a device ID.
  static const int MaxPins = 8;


  // Initialize the DeviceID class with a default device identifier.
  // The optional powerdelay is the delay in milliseconds between
  // switching on power and reading out the switches.
  DeviceID(int id=0, int powerdelay=2);

  // Return the device ID.
  int id() const { return ID; };

  // Set the device identifier to id.
  void setID(int id);

  // Replace in str:
  // - "ID" by the device identifier, e.g. "103", "14", "7".
  //   Use "ID2" or "ID3" to pad the ID with zeros to two or three digits,
  //   e.g. "103", "14", "07" for "ID2" or "103", "014", "007" of "ID3".
  // - "IDA" by the device identifier as alphabetical characters,
  //   e.g. "DY", "N", "G".
  //   Use "IDAA" to always use 2 characters: e.g. "DY", "AN", "AG".
  String makeStr(const String &str) const;

  // Print device identifier on stream.
  void report(Stream &stream=Serial) const;

  // Set the pins that have switches connected.
  // The first pin in the array is bit 0.
  // Terminate the array with a negative number.
  void setPins(const int *pins);

  // Set the powerpin that supplies the switches with power and
  // the pins that have switches connected.
  // The first pin in the array is bit 0.
  // Terminate the array with a negative number.
  void setPins(int powerpin, const int *pins);

  // Read out the device ID from DIP switches.
  // That is, supply power via the power pin, sequentally read in the
  // states of the pins, and return the number encoded by these bits.
  // If no pins were specified or all pins are zero, the do no set the
  // device ID and return -1.
  // On success returns the read in device ID.
  int read();

  
protected:
  
  int ID;             // the device ID.
  int Source;         // from which source was the device identifier set:
                      // 0: none, 1: default via the constructor, 2: set via setID(), 3: read in via read().

  int NPins;          // number of pins.
  int Pins[MaxPins];  // the pins.
  int PowerPin;       // the pin providing power to the switches.
  int PowerDelay;     // the delay in ms between providing power and reading out the pins (default 2ms).

};


#endif
