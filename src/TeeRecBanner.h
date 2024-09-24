/*
  TeeRecBanner - ASCII art banner for serial output.
  Created by Jan Benda, September 23th, 2024.
*/

#ifndef TeeRecBanner_h
#define TeeRecBanner_h


#include <Arduino.h>


#define TEEREC_SOFTWARE "TeeRec version 0.10.0"


void printTeeRecBanner(const char *software=NULL, Stream &stream=Serial);


#endif

