#include <Arduino.h>
#include <DataBuffer.h>
#include "AudioMonitor.h"


AudioPlayBuffer::AudioPlayBuffer(const DataWorker &producer)
  : DataWorker(&producer),
    AudioStream(0, NULL),
    Time(0.0),
    VolShift(1),
    Mute(false) {
}


AudioPlayBuffer::~AudioPlayBuffer() {
}


void AudioPlayBuffer::update() {
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
    block1->data[i] = left >> VolShift;
    if (numConnections > 1)
      block2->data[i] = right >> VolShift;
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


void AudioPlayBuffer::mixer(int16_t &left, int16_t &right) {
  uint8_t nchannels = Data->nchannels();
  int16_t val = 0;
  for (uint8_t c=0; c<nchannels; c++)
    val += Data->buffer()[Index+c]/nchannels;
  left = val;
  right = val;
}


void AudioPlayBuffer::setVolume(uint8_t shift) {
  VolShift = shift;
}


void AudioPlayBuffer::setMute(bool mute) {
  Mute = mute;
}


AudioMonitor::AudioMonitor(AudioPlayBuffer *audiodata) :
  Own(false),
  AudioInput(audiodata),
  PatchCord1(0),
  PatchCord2(0) {
}


AudioMonitor::AudioMonitor(const DataWorker *producer) :
  Own(true),
  PatchCord1(0),
  PatchCord2(0) {
  AudioInput = new AudioPlayBuffer(*producer);
}


AudioMonitor::~AudioMonitor() {
  if (PatchCord1 != 0)
    delete PatchCord1;
  if (PatchCord2 != 0)
    delete PatchCord2;
  if (Own)
    delete AudioInput;
}


void AudioMonitor::setup(bool stereo, int enable_pin) {
  AudioMemory(32);
  PatchCord1 = new AudioConnection(*AudioInput, 0, AudioOutput, 0);
  if (stereo)
    PatchCord2 = new AudioConnection(*AudioInput, 1, AudioOutput, 1);
  if ( enable_pin >= 0 ) {
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, HIGH); // turn on the amplifier
    delay(10);                      // allow time to wake up
  }
}

