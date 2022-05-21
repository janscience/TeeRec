/*
  AudioMonitor - play acquired data on audio I2S device.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioMonitor_h
#define AudioMonitor_h


#include <Arduino.h>
#include <Audio.h>
#include <DataWorker.h>


class AudioPlayBuffer : public DataWorker, public AudioStream {
  
 public:
  
  AudioPlayBuffer(const DataWorker &producer);
  virtual ~AudioPlayBuffer();
  
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


class AudioMonitor {

 public:

  // Constructor with defaul AudioPlayBuffer that simply averages all
  // data channels.
  AudioMonitor(const DataWorker *producer);

  // Constructor that lets you also pass a derived AudioPlayBuffer.
  AudioMonitor(AudioPlayBuffer *audiodata);

  ~AudioMonitor();

  void setup(bool stereo=true, int enable_pin=-1);


 protected:

  bool Own;
  AudioPlayBuffer *AudioInput;
  AudioOutputI2S AudioOutput;
  AudioConnection *PatchCord1;
  AudioConnection *PatchCord2;

};

#endif
