/*
  AudioShield - playing acquired data on audio shield.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioShield_h
#define AudioShield_h


#include <Arduino.h>
#include <AudioStream.h>
#include <ContinuousADC.h>


class AudioPlayBuffer : public DataConsumer, public AudioStream {
  
 public:
  
  AudioPlayBuffer(const DataBuffer &data);
  virtual void update();

 private:

  double LPVals[16];
  
};

/*
class AudioShield {

 public:

  AudioShield(const DataBuffer &data);
  ~AudioShield();

  void setup();


 protected:

  static const size_t NAudioMem = 8;
  static DMAMEM audio_block_t AudioMem[NAudioMem];
  
  AudioPlayBuffer *AudioInput;
  AudioOutputI2S AudioOutput;
  AudioConnection *PatchCord1;
  AudioConnection *PatchCord2;
  AudioControlSGTL5000 Shield;

};
*/

#endif
