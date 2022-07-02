/*
  AudioPlayBuffer - make the TeeRec data buffer available as an input for the Audio library
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioPlayBuffer_h
#define AudioPlayBuffer_h


#include <Arduino.h>
#include <Audio.h>
#include <DataWorker.h>


class AudioPlayBuffer : public DataWorker, public AudioStream {
  
 public:
  
  AudioPlayBuffer();
  AudioPlayBuffer(const DataWorker &producer);
  virtual ~AudioPlayBuffer();
  
  virtual void update();

  // Set low-pass filter time-constant to n/44.1kHz, default is 10.
  // Must be larger or equal to one.
  void setLowpass(int16_t n);

  void setMute(bool mute=true);

  typedef void (AudioPlayBuffer::*MixerFunc)(int16_t &left, int16_t &right);
  
  // Compute the average of all channels at the current index of the
  // data buffer and copy it to both the left and right channel.
  void average(int16_t &left, int16_t &right);
  
  // Subtract the second data channel at the current Index from the
  // first.  Divide by two and assign this to the left audio channel
  // and the negative to the right one.
  void difference(int16_t &left, int16_t &right);
  
  // Assign the first channel at the current index of the data buffer
  // to left, and the second one to the right audio channel.
  void assign(int16_t &left, int16_t &right);
  
  // Assign one of the above functions to mixer for mapping all
  // channels of the data producer to the left and right channel of
  // audio output.  By default the average() function is assigned..
  MixerFunc mixer;

  
 protected:

  float Time;
  bool Mute;
  int16_t LeftVal;
  int16_t RightVal;
  int16_t LowpassN;
  
};


#endif
