/*
  TeeRecBanner - ASCII art banner and TeeRec version for output streams.
  Created by Jan Benda, September 23th, 2024.
*/

#ifndef TeeRecBanner_h
#define TeeRecBanner_h


#include <Arduino.h>


#define TEEREC_SOFTWARE "TeeRec version 0.11.0"


void printTeeRecBanner(const char *software=NULL, Stream &stream=Serial);


#endif

