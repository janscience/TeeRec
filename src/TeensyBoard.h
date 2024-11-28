/*
  TeensyBoard - find out which Teensy board we are running on.
  Created by Jan Benda, June 4th, 2021.
*/

#ifndef TeensyBoard_h
#define TeensyBoard_h


#include <Arduino.h>


// From https://github.com/pedvide/ADC/blob/master/settings_defines.h

#if defined(TEENSYDUINO) 
  #if defined(__MKL26Z64__) || defined(KINETISL)  // Teensy LC
    #define TEENSYLC
  #elif defined(KINETISK)      // Teensy 3.X
    #define TEENSY3
    #if defined(__MK20DX256__)   // Teensy 3.1/3.2
      #define TEENSY32
    #elif defined(__MK20DX128__) // Teensy 3.0
      #define TEENSY30
    #elif defined(__MK64FX512__) // Teensy 3.5
      #define TEENSY35
    #elif defined(__MK66FX1M0__) // Teensy 3.6
      #define TEENSY36
    #else
      #error "Unknown Teensy 3.X board!"
    #endif
  #elif defined(__IMXRT1062__) // Teensy 4.0/4.1
    // They only differ in the number of exposed pins
    #define TEENSY4
    #ifdef ARDUINO_TEENSY41
      #define TEENSY41
    #else
      #define TEENSY40
    #endif
  #else
    #error "Unknown Teensy board!"
  #endif
#else
  #error "Non Teensy boards not supported"
#endif

// Return string with name of Teensy board.
const char *teensyBoard();

// Return string with CPU speed of Teensy board in MHz.
long teensySpeed();

// Set the speed of the CPU in MHz.
void setTeensySpeed(long speed);


// From https://github.com/sstaub/TeensyID :

// Pointer to an  array with size of 4 uint8_t containing the serial number
void teensySN(uint8_t *sn);

// Serial number as a formated string in hex xx-xx-xx-xx
const char *teensySN(void);


// Pointer to an array with size of 6 uint8_t containing the MAC address
void teensyMAC(uint8_t *mac);

// MAC address as formated string in hex xx:xx:xx:xx:xx:xx
const char *teensyMAC(void);

// analog pins:
#if !defined(PIN_A13)
const int NAPins = 13;
const int APins[NAPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12};
#elif !defined(PIN_A14)
const int NAPins = 14;
const int APins[NAPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13};
#elif !defined(PIN_A18)
const int NAPins = 18;
const int APins[NAPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, A17};
#elif !defined(PIN_A21)
const int NAPins = 21;
const int APins[NAPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
			   A13, A14, A15, A16, A17, A18, A19, A20};
#else
const int NAPins = 27;
const int APins[NAPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
			   A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24, A25, A26};
#endif


// Return in pins the name of the analog pin (e.g. "A22", max 4 characters).
// Returns the number of characters written into pins.
int analogPin(int8_t pin, char *pins);

#endif
