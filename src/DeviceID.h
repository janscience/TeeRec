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
  DeviceID(int id=0);

  // Set the pins that have switches connected.
  // The first pin in the array is bit 0.
  // Terminate the array with a negative number.
  void set(const int *pins);

  // Set the powerpin that supplies the switches with power and
  // the pins that have switches connected.
  // The first pin in the array is bit 0.
  // Terminate the array with a negative number.
  void set(int powerpin, const int *pins);

  // Set the powerpin that supplies the switches with power,
  // the delay after with the pins are read out in ms, and
  // the pins that have switches connected.
  // The first pin in the array is bit 0.
  // Terminate the array with a negative number.
  void set(int powerpin, int powerdelay, const int *pins);

  // Read out the device ID.
  // That is, supply power via the power pin, sequentally read in the
  // states of the pins, and return the number encoded by these bits.
  int read();

  // Return the previously read in device ID.
  int id() const { return ID; };

  // Replace in str:
  // - "ID3" by the device identifier padded with zeros to three digits,
  //   e.g. "103", "014", "007".
  // - "ID2" by the device identifier padded with zeros to two digits,
  //   e.g. "103", "14", "07".
  // - "ID" by the device identifier,
  //   e.g. "103", "14", "7".
  // - "IDAA" by the device identifier as two alphanumerical characters,
  //   e.g. "DY", "AN", "AH".
  // - "IDAA" by the device identifier as one or two alphanumerical characters,
  //   e.g. "DY", "N", "H".
  String makeStr(const String &str) const;

  
protected:

  int NPins;          // number of pins.
  int Pins[MaxPins];  // the pins.
  int PowerPin;       // the pin providing power to the switches.
  int PowerDelay;     // the delay in ms between providing power and reading out the pins (default 2ms).
  int ID;       // the read out device ID.

};


#endif
