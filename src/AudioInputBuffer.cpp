#include <Arduino.h>
#include <DataBuffer.h>
#include "AudioInputBuffer.h"


AudioInputBuffer::AudioInputBuffer(const DataWorker &producer)
  : DataWorker(&producer),
    AudioStream(0, NULL),
    Time(0.0),
    Mute(false) {
}


AudioInputBuffer::~AudioInputBuffer() {
}


void AudioInputBuffer::update() {
  // this function should be as fast as possible!
  
  if (Mute)
    return;
  
  audio_block_t *block1 = NULL;
  audio_block_t *block2 = NULL;

  ssize_t navail = available();
  double interval = 1.0/AUDIO_SAMPLE_RATE_EXACT;
  if (Data->time(navail) < AUDIO_BLOCK_SAMPLES*interval)
    return;
  
  // allocate audio blocks to transmit:
  block1 = allocate();
  if (block1 == NULL)
    return;
  if (numConnections > 1) {
    block2 = allocate();
    if (block2 == NULL)
      return;
  }
  
  // copy data into audio block buffer:
  uint8_t nchannels = Data->nchannels();
  ssize_t start = Index;
  int16_t left;
  int16_t right;
  unsigned int i = 0;
  while (i<AUDIO_BLOCK_SAMPLES) {
    mixer(left, right);
    block1->data[i] = left;
    if (numConnections > 1)
      block2->data[i] = right;
    i++;
    Time += interval;
    while (navail > 0 && Time > Data->time(Index - start + nchannels)) {
      navail -= nchannels;
      if (increment(nchannels))
	start -= Data->nbuffer();
    }
  }
  Time -= Data->time(Index - start);  // keep time mismatch!

  transmit(block1, 0);
  release(block1);
  if (numConnections > 1) {
    transmit(block2, 1);
    release(block2);
  }
}


void AudioInputBuffer::mixer(int16_t &left, int16_t &right) {
  uint8_t nchannels = Data->nchannels();
  int16_t val = 0;
  for (uint8_t c=0; c<nchannels; c++)
    val += Data->buffer()[Index+c]/nchannels;
  left = val;
  right = val;
}


void AudioInputBuffer::setMute(bool mute) {
  Mute = mute;
}
