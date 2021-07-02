/*
  AudioShield - playing acquired data on audio shield.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioShield_h
#define AudioShield_h


#include <Arduino.h>
#include <AudioStream.h>


class AudioPlayBuffer : public AudioStream {
  
 public:
  
  AudioPlayBuffer();
  void begin();
  virtual void update();

	
 private:
  
  // volatile bool Playing;
  
};


class AudioShield {

 public:

  AudioShield();


};


#endif
