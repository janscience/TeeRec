/*
  AudioShield - playing acquired data on audio shield.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioShield_h
#define AudioShield_h


#include <Arduino.h>
#include <Audio.h>
#include <DataWorker.h>


class AudioPlayBuffer : public DataWorker, public AudioStream {
  
 public:
  
  AudioPlayBuffer(const DataWorker &producer);
  virtual void update();

 private:

  double Time;
  double LPVals[16];
  
};


class AudioShield {

 public:

  AudioShield(const DataWorker &producer);
  ~AudioShield();

  void setup();


 protected:

  AudioPlayBuffer *AudioInput;
  AudioOutputI2S AudioOutput;
  AudioConnection *PatchCord1;
  AudioConnection *PatchCord2;
  AudioControlSGTL5000 Shield;

};

#endif
