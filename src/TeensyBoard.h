/*
  TeensyBoard - find out which Teensy board we are running on.
  Created by Jan Benda, June 4th, 2021.
*/

#ifndef TeensyBoard_h
#define TeensyBoard_h


#include <Arduino.h>


// from https://arduino.stackexchange.com/questions/21137/arduino-how-to-get-the-board-type-in-code

#if defined(TEENSYDUINO) 
  #if defined(__AVR_ATmega32U4__)
    #define TEENSY20
  #elif defined(__AVR_AT90USB1286__)       
    #define TEENSY20PP
  #elif defined(__MK20DX128__)       
    #define TEENSY30
  #elif defined(__MK20DX256__)       
    #define TEENSY32             // and Teensy 3.1 (obsolete)
  #elif defined(__MKL26Z64__)       
    #define TEENSYLC
  #elif defined(__MK64FX512__)
    #define TEENSY35
  #elif defined(__MK66FX1M0__)
    #define TEENSY36
  #else
    // How to figure out Teensy 4.0 and 4.1 ?
    #error "Unknown Teensy board"
  #endif
#else
  #error "Non Teensy boards not supported"
#endif


#endif
