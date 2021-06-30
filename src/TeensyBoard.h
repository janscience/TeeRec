/*
  TeensyBoard - find out which Teensy board we are running on.
  Created by Jan Benda, June 4th, 2021.
*/

#ifndef TeensyBoard_h
#define TeensyBoard_h


#include <Arduino.h>


// From https://github.com/pedvide/ADC/blob/master/settings_defines.h

#if defined(TEENSYDUINO) 
  #if defined(__MK20DX256__)   // Teensy 3.1/3.2
    #define TEENSY31
  #elif defined(__MK20DX128__) // Teensy 3.0
    #define TEENSY30
  #elif defined(__MKL26Z64__)  // Teensy LC
    #define TEENSYLC
  #elif defined(__MK64FX512__) // Teensy 3.5
    #define TEENSY35
  #elif defined(__MK66FX1M0__) // Teensy 3.6
    #define TEENSY36
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


// From https://github.com/sstaub/TeensyID :

// Pointer to an  array with size of 4 uint8_t containing the serial number
void teensySN(uint8_t *sn);

// Serial number as a formated string in hex xx-xx-xx-xx
const char *teensySN(void);


// Pointer to an array with size of 6 uint8_t containing the MAC address
void teensyMAC(uint8_t *mac);

// MAC address as formated string in hex xx:xx:xx:xx:xx:xx
const char *teensyMAC(void);

#endif
