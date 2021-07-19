#include <Arduino.h>
#include "AudioShield.h"


AudioPlayBuffer::AudioPlayBuffer(const DataBuffer &data)
  : DataConsumer(&data),
    AudioStream(0, NULL) {
  memset(LPVals, 0, sizeof(LPVals));
}


void AudioPlayBuffer::update() {
  audio_block_t *block1 = NULL;
  //audio_block_t *block2 = NULL;

  if (Data->nchannels() == 0 || Data->rate() == 0)
    return;

  double interval = 1.0/AUDIO_SAMPLE_RATE_EXACT;
  if (Data->time(available()) < (AUDIO_BLOCK_SAMPLES+2)*interval)
    return;

  size_t head = Data->head();
  if (head == 0)
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

  int32_t nchannels = Data->nchannels();
  float rate = Data->rate();
  double fac = 1.0/rate/(20.0*interval);  // make sure fac < 0.1
  if (fac > 0.05)
    fac = 0.0;
  double time = 0.0;
  ssize_t start = Tail;
  unsigned int i = 0;
  while (i<AUDIO_BLOCK_SAMPLES) {
    int32_t sum = 0;
    for (uint8_t c=0; c<Data->nchannels(); c++)
      sum += Data->buffer()[Tail+c] - LPVals[c];
    block1->data[i++] = sum/nchannels;
    time += interval;
    while (Data->samples(time) > Tail - start + nchannels) {
      for (uint8_t c=0; c<Data->nchannels(); c++)
	LPVals[c] += (Data->buffer()[Tail+c] - LPVals[c])*fac;
      Tail += nchannels;
      if (Tail >= Data->nbuffer()) {
	Tail -= Data->nbuffer();
	start -= Data->nbuffer();
	TailCycle++;
      }
    }
  }
  
  /*
  if (Data->nchannels() == 1)
    memcpy(block2, block1, 2*AUDIO_BLOCK_SAMPLES);
  */

  transmit(block1, 0);
  transmit(block1, 1);
  release(block1);
  //release(block2);
}


AudioShield::AudioShield(const DataBuffer &data) {
  AudioInput = new AudioPlayBuffer(data);
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

