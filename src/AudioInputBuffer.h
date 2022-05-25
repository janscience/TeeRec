/*
  AudioInputBuffer - make the TeeRec data buffer available as an input for the Audio library
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioInputBuffer_h
#define AudioInputBuffer_h


#include <Arduino.h>
#include <Audio.h>
#include <DataWorker.h>


class AudioInputBuffer : public DataWorker, public AudioStream {
  
 public:
  
  AudioInputBuffer(const DataWorker &producer);
  virtual ~AudioInputBuffer();
  
  virtual void update();

  void setMute(bool mute=true);
  

 protected:

  // Reimplement this function to map from all channels of the data
  // producer to the left and right channel of audio output.
  // This default implementation just takes the average of all channels
  // and copies it to both the left and right channel.
  virtual void mixer(int16_t &left, int16_t &right);

  float Time;
  bool Mute;
  
};


#endif
