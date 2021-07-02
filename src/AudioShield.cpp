#include <Arduino.h>
#include "AudioShield.h"


AudioPlayBuffer::AudioPlayBuffer()
  : AudioStream(0, NULL) {
  NChannels = 0;
  Rate = 0;
}


void AudioPlayBuffer::reset() {
  DataConsumer::reset();
  NChannels = ContinuousADC::ADCC->nchannels();
  Rate = ContinuousADC::ADCC->rate();
}


void AudioPlayBuffer::update() {
  audio_block_t *block;

  if (NChannels == 0 || Rate == 0)
    return;
  
  // allocate the audio blocks to transmit
  block = allocate();
  if (block == NULL)
    return;

  // transfer data from buffer to block:
  unsigned int i = 0;
  if (Tail > Data->Head) {
    for (i=0; i<AUDIO_BLOCK_SAMPLES && Tail < Data->NBuffer; i++) {
      block->data[i] = Data->Buffer[Tail++];
    }
    if (Tail >= Data->NBuffer)
      Tail = 0;
  }
  if (Tail < Data->Head) {
    for ( ; i<AUDIO_BLOCK_SAMPLES && Tail < Data->Head; i++) {
      block->data[i] = Data->Buffer[Tail++];
    }
  }
  for ( ; i<AUDIO_BLOCK_SAMPLES; i++)
    block->data[i] = 0;
  
  transmit(block);
  release(block);
}


AudioShield::AudioShield() {
}


AudioShield::~AudioShield() {
  delete PatchCord1;
  delete PatchCord2;
}


void AudioShield::setup() {
  AudioMemory(8);

  PatchCord1 = new AudioConnection(AudioInput, 0, AudioOutput, 0);
  //PatchCord2 = new AudioConnection(AudioInput, 1, AudioOutput, 1);
  
  Shield.enable();
  Shield.volume(0.5);
}

