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
  
  block1 = allocate();
  if (block1 == NULL)
    return;

  unsigned int i = 0;
  while (i<AUDIO_BLOCK_SAMPLES) {
    //float x = sin(TWO_PI*(i%64)/64);
    //block1->data[i++] = int16_t(4000*x);
    block1->data[i++] = 0;
  }

  transmit(block1, 0);
  release(block1);

  return;
  /*
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
  */

  /*
  if (Data->nchannels() > 1) {
    block2 = allocate();
    if (block2 == NULL)
      return;
  }
  */

  /*
  // transfer data from buffer to blocks:
  int32_t nchannels = Data->nchannels();
  float rate = Data->rate();
  double fac = 1.0/rate/(20.0*interval);  // make sure fac < 0.1
  ssize_t tail = Tail;
  double time = 0.0;
  unsigned int i = 0;
  if ( fac <= 0.02 ) {
    while (i<AUDIO_BLOCK_SAMPLES) {
      int32_t sum = 0;
      for (uint8_t c=0; c<Data->nchannels(); c++)
	sum += LPVals[c];
      block1->data[i++] = sum/nchannels;
      time += interval;
      size_t nexttail = tail + Data->samples(time);
      while (Tail < nexttail) {
	for (uint8_t c=0; c<Data->nchannels(); c++) {
	  LPVals[c] += (Data->buffer()[Tail++] - LPVals[c])*fac;
	  //LPVals[c] = Data->buffer()[Tail++];
	}
	if (Tail >= Data->nbuffer()) {
	  Tail -= Data->nbuffer();
	  tail -= Data->nbuffer();
	  nexttail -= Data->nbuffer();
	  TailCycle++;
	}
      }
    }
  }
  else if (rate <= AUDIO_SAMPLE_RATE_EXACT) {
    // linear interpolation:
    while (i<AUDIO_BLOCK_SAMPLES) {
      int32_t sum = 0;
      for (uint8_t c=0; c<Data->nchannels(); c++) {
	float diff = Data->buffer()[Tail+nchannels+c] - Data->buffer()[Tail+c];
	sum += round(diff*rate*(time - Data->time(Tail-tail)) + Data->buffer()[Tail+c]);
      }
      block1->data[i++] = sum/nchannels;
      time += interval;
      if (Data->samples(time) > Tail - tail + nchannels) {
	Tail += nchannels;
	if (Tail >= Data->nbuffer()) {
	  Tail -= Data->nbuffer();
	  tail -= Data->nbuffer();
	  TailCycle++;
	}
      }
    }
  }
  else {
    while (i<AUDIO_BLOCK_SAMPLES) {
      float x = sin(TWO_PI*(i%64)/64);
      block1->data[i++] = int16_t(4000*x);
      //block1->data[i++] = 0;
    }
  }
  */
  /*
  if (Data->nchannels() == 1)
    memcpy(block2, block1, 2*AUDIO_BLOCK_SAMPLES);
  */

  /*
  transmit(block1, 0);
  //transmit(block1, 1);
  //transmit(block2, 1);
  release(block1);
  //release(block2);
  */
}

/*
DMAMEM audio_block_t AudioShield::AudioMem[NAudioMem];


AudioShield::AudioShield(const DataBuffer &data) {
  AudioInput = new AudioPlayBuffer(data);
}


AudioShield::~AudioShield() {
  delete PatchCord1;
  // delete PatchCord2;
  // delete AudioInput; ????
}


void AudioShield::setup() {
  PatchCord1 = new AudioConnection(*AudioInput, 0, AudioOutput, 0);
  //  PatchCord2 = new AudioConnection(*AudioInput, 1, AudioOutput, 1);
  AudioStream::initialize_memory(AudioMem, NAudioMem);
  Shield.enable();
  Shield.volume(0.5);
}
*/
