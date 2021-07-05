/*
  AudioShield - playing acquired data on audio shield.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioShield_h
#define AudioShield_h


#include <Arduino.h>
#include <Audio.h>
#include <ContinuousADC.h>


class AudioPlayBuffer : public DataConsumer, public AudioStream {
  
 public:
  
  AudioPlayBuffer();
  virtual void update();
  virtual void reset();


private:

  volatile uint8_t NChannels;
  volatile uint32_t Rate;
  
};


class AudioShield {

 public:

  AudioShield();
  ~AudioShield();

  void setup();


 protected:

  AudioPlayBuffer AudioInput;
  AudioOutputI2S AudioOutput;
  AudioConnection *PatchCord1;
  AudioConnection *PatchCord2;
  AudioControlSGTL5000 Shield;

};


#endif