#include <Arduino.h>
#include <DataBuffer.h>
#include "AudioShield.h"


AudioPlayBuffer::AudioPlayBuffer(const DataWorker &producer)
  : DataWorker(&producer),
    AudioStream(0, NULL),
    Time(0.0) {
  memset(LPVals, 0, sizeof(LPVals));
}


void AudioPlayBuffer::update() {
  audio_block_t *block1 = NULL;
  //audio_block_t *block2 = NULL;

  if (Data->nchannels() == 0 || Data->rate() == 0)
    return;

  ssize_t navail = available();
  double interval = 1.0/AUDIO_SAMPLE_RATE_EXACT;
  if (Data->time(navail) < AUDIO_BLOCK_SAMPLES*interval)
    return;
  
  // allocate audio blocks to transmit:
  block1 = allocate();
  if (block1 == NULL)
    return;
  /*
  if (Data->nchannels() > 1) {
    block2 = allocate();
    if (block2 == NULL)
      return;
  }
  */

  // low-pass filter:
  const float tau = 0.1;             // low-pass filter time constant in seconds
  double fac = 1.0/Data->rate()/tau; // fac = dt/tau = 1/rate/tau
  if (fac > 0.05)                    // make sure fac < 0.1
    fac = 0.0;
  
  // copy data into audio block buffer:
  int32_t nchannels = Data->nchannels();
  ssize_t start = Index;
  unsigned int i = 0;
  while (i<AUDIO_BLOCK_SAMPLES) {
    int32_t sum = 0;
    for (uint8_t c=0; c<Data->nchannels(); c++)
      sum += Data->buffer()[Index+c] - LPVals[c];
    block1->data[i++] = sum/nchannels;
    Time += interval;
    while (navail > 0 && Time > Data->time(Index - start + nchannels)) {
      for (uint8_t c=0; c<Data->nchannels(); c++)
	LPVals[c] += (Data->buffer()[Index+c] - LPVals[c])*fac;
      navail -= nchannels;
      if (increment(nchannels))
	start -= Data->nbuffer();
    }
  }
  Time -= Data->time(Index - start);
  
  /*
  if (Data->nchannels() == 1)
    memcpy(block2, block1, 2*AUDIO_BLOCK_SAMPLES);
  */

  transmit(block1, 0);
  transmit(block1, 1);
  release(block1);
  //release(block2);
}


AudioShield::AudioShield(const DataWorker &producer) {
  AudioInput = new AudioPlayBuffer(producer);
}


AudioShield::~AudioShield() {
  delete PatchCord1;
  delete PatchCord2;
  // delete AudioInput; ????
}


void AudioShield::setup() {
  AudioMemory(8);
  PatchCord1 = new AudioConnection(*AudioInput, 0, AudioOutput, 0);
  PatchCord2 = new AudioConnection(*AudioInput, 1, AudioOutput, 1);
  Shield.enable();
  Shield.volume(0.5);
}

