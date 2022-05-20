#include <Arduino.h>
#include <DataBuffer.h>
#include "AudioShield.h"


AudioPlayBuffer::AudioPlayBuffer(const DataWorker &producer)
  : DataWorker(&producer),
    AudioStream(0, NULL),
    Time(0.0),
    Mute(false) {
  memset(LPVals, 0, sizeof(LPVals));
}


AudioPlayBuffer::~AudioPlayBuffer() {
}


void AudioPlayBuffer::update() {
  if (Mute)
    return;
  
  audio_block_t *block1 = NULL;
  audio_block_t *block2 = NULL;

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
  block2 = allocate();
  if (block2 == NULL)
    return;

  // low-pass filter:
  const float tau = 0.1;             // low-pass filter time constant in seconds
  double fac = 1.0/Data->rate()/tau; // fac = dt/tau = 1/rate/tau
  if (fac > 0.05)                    // make sure fac < 0.1
    fac = 0.0;
  
  // copy data into audio block buffer:
  uint8_t nchannels = Data->nchannels();
  ssize_t start = Index;
  unsigned int i = 0;
  while (i<AUDIO_BLOCK_SAMPLES) {
    int16_t left = 0;
    int16_t right = 0;
    mixer(left, right);
    block1->data[i] = left;
    block2->data[i] = right;
    i++;
    Time += interval;
    while (navail > 0 && Time > Data->time(Index - start + nchannels)) {
      for (uint8_t c=0; c<nchannels; c++)
	LPVals[c] += (Data->buffer()[Index+c] - LPVals[c])*fac;
      navail -= nchannels;
      if (increment(nchannels))
	start -= Data->nbuffer();
    }
  }
  Time -= Data->time(Index - start);  // keep time mismatch!

  transmit(block1, 0);
  transmit(block2, 1);
  release(block1);
  release(block2);
}


void AudioPlayBuffer::mixer(int16_t &left, int16_t &right) {
  uint8_t nchannels = Data->nchannels();
  int16_t val = 0;
  for (uint8_t c=0; c<nchannels; c++)
    val += (Data->buffer()[Index+c] - LPVals[c])/nchannels;
  left = val;
  right = val;
}


void AudioPlayBuffer::setMute(bool mute) {
  Mute = mute;
}


AudioShield::AudioShield(AudioPlayBuffer *audiodata) :
  Own(false),
  AudioInput(audiodata) {
}


AudioShield::AudioShield(const DataWorker *producer) :
  Own(true) {
  AudioInput = new AudioPlayBuffer(*producer);
}


AudioShield::~AudioShield() {
  delete PatchCord1;
  delete PatchCord2;
  if (Own)
    delete AudioInput;
}


void AudioShield::setup() {
  AudioMemory(32);
  PatchCord1 = new AudioConnection(*AudioInput, 0, AudioOutput, 0);
  PatchCord2 = new AudioConnection(*AudioInput, 1, AudioOutput, 1);

  /*
  AudioOutput.analogReference(EXTERNAL); // much louder!
  delay(50);
  */
  int amp_pin = 32;
  pinMode(amp_pin, OUTPUT);
  digitalWrite(amp_pin, HIGH); // turn on the amplifier
  delay(10);             // allow time to wake up

  Shield.enable();
  Shield.volume(0.5);
  //Shield.muteHeadphone();
  //Shield.muteLineout();
  Shield.lineOutLevel(31);
}

