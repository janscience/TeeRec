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
  audio_block_t *block1 = NULL;
  audio_block_t *block2 = NULL;

  if (NChannels == 0 || Rate == 0)
    return;

  if (available()/NChannels*(1.0/Rate) < AUDIO_BLOCK_SAMPLES/AUDIO_SAMPLE_RATE_EXACT)
    return;
  
  // allocate audio blocks to transmit:
  block1 = allocate();
  if (block1 == NULL)
    return;
  if (NChannels > 1) {
    block2 = allocate();
    if (block2 == NULL)
      return;
  }

  // transfer data from buffer to blocks:
  for (uint8_t c=0; c<NChannels; c++) {
    unsigned int i = 0;
    if (Tail > Data->Head) {
      for (i=0; i<AUDIO_BLOCK_SAMPLES && Tail < Data->NBuffer; i++) {
	block1->data[i] = Data->Buffer[Tail++];
      }
      if (Tail >= Data->NBuffer)
	Tail = 0;
    }
    if (Tail < Data->Head) {
      for ( ; i<AUDIO_BLOCK_SAMPLES && Tail < Data->Head; i++) {
	block1->data[i] = Data->Buffer[Tail++];
      }
    }
    // fill up with zeros: that should actually not be needed!
    for ( ; i<AUDIO_BLOCK_SAMPLES; i++)
      block1->data[i] = 0;
  }

  if (NChannels == 1)
    memcpy(block2, block1, 2*AUDIO_BLOCK_SAMPLES);

  transmit(block1, 0);
  transmit(block2, 1);
  release(block1);
  release(block2);
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

