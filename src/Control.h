/*
  Control - Minimum interface to classes controlling data acquisition.
  Created by Jan Benda, August 20th, 2023.
*/

#ifndef Control_h
#define Control_h


#include <Arduino.h>


class Control {
  
public:

  /* Return the gain set for all channels in dB. */
  virtual float gain() = 0;

  /* Return the current gain as a string in gains.
     The gain of the acquisition system is multiplied with pregain. */
  virtual void gainStr(char *gains, float pregain=1.0) = 0;

  /* Set gain of all channels to gain in dB. */
  virtual bool setGain(float gain) = 0;
  
};


#endif

