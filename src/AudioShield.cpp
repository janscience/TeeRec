#include <Arduino.h>
#include "AudioShield.h"


AudioPlayBuffer::AudioPlayBuffer()
  : AudioStream(0, NULL) {
  begin();
}


void AudioPlayBuffer::begin() {
}


void AudioPlayBuffer::update() {
  audio_block_t *block;

  // if not connected to buffer, return.
  
  // allocate the audio blocks to transmit
  block = allocate();
  if (block == NULL)
    return;

  // transfer data from buffer to block:
  for (unsigned int i=0; i<AUDIO_BLOCK_SAMPLES; i++) {
    block->data[i] = 0;
  }
  
  transmit(block);
  release(block);
}


AudioShield::AudioShield() {
}

