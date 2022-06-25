#include "AudioMonitor.h"


AudioMonitor::AudioMonitor(DataWorker &data, AudioStream &speaker) :
  Data(data),
  NSounds(0),
  Volume(0.8),
  VolumeButtons(false) {
  Mixer = new AudioMixer4;
  ACI = new AudioConnection(Data, 0, *Mixer, 0);
  ACO = new AudioConnection(*Mixer, 0, speaker, 0);
  Gains[0] = 1.0;
  for (size_t i=0; i<3; i++) {
    Gains[i+1] = 0.0;
    Beep[i] = 0;
    BeepInterval[i] = 0;
  }
  Play = false;
  AmplifierPin = -1;
}


void AudioMonitor::setupAmp(int amplifier_pin, int mode) {
  Play = true;
  if ( amplifier_pin >= 0 ) {
    AmplifierPin = amplifier_pin;
    pinMode(AmplifierPin, OUTPUT);
    digitalWrite(AmplifierPin, HIGH); // turn on the amplifier
    delay(10);                        // allow time to wake up
  }
}


void AudioMonitor::setupVolume(float volume, int volume_up_pin,
			       int volume_down_pin, int mode) {
  Play = true;
  Volume = volume;
  setVolume();
  if (volume_up_pin >= 0 && volume_down_pin >= 0) {
    VolumeUpButton.attach(volume_up_pin, mode);
    VolumeUpButton.setPressedState(mode==INPUT_PULLUP?LOW:HIGH);
    VolumeUpButton.interval(20);
    VolumeDownButton.attach(volume_down_pin, mode);
    VolumeDownButton.setPressedState(mode==INPUT_PULLUP?LOW:HIGH);
    VolumeDownButton.interval(20);
    VolumeButtons = true;
  }
}


void AudioMonitor::pause() {
  Play = false;
  Data.setMute(true);
}


void AudioMonitor::play() {
  Play = true;
  Data.setMute(false);
}


void AudioMonitor::addFeedback(float gain, float freq, float duration) {
  Sound[NSounds] = new AudioPlayMemory;
  ACS[NSounds] = new AudioConnection(*Sound[NSounds], 0, *Mixer, NSounds+1);
  Gains[0] -= gain;
  if (Gains[0] < 0.1)
    Gains[0] = 0.1;
  Gains[NSounds+1] = gain;
  // generate beep waveform:
  size_t np = size_t(AUDIO_SAMPLE_RATE_EXACT/freq);
  unsigned int n = (unsigned int)(duration*AUDIO_SAMPLE_RATE_EXACT);
  //  unsigned int n = (unsigned int)(duration*AUDIO_SAMPLE_RATE_EXACT/np)*np;
  Beep[NSounds] = new int16_t[2+n];
  // first integer encodes format and size:
  unsigned int format = 0x81;
  format <<= 24;
  format |= n;
  Beep[NSounds][0] = format & 0xFFFF;
  Beep[NSounds][1] = format >> 16;
  // Gabor sine tone:
  uint16_t a = 1 << 15;
  for (size_t i=0; i<n; i++)
    Beep[NSounds][2+i] = (int16_t)(a*0.5*(1.0-cos(TWO_PI*i/n))*sin(TWO_PI*i/np));
    // Beep[NSounds][2+i] = (int16_t)(a*exp(-0.5*pow((i-n/2)/(n/4), 2))*sin(TWO_PI*i/np));
  
  NSounds++;
  setVolume();  
}


void AudioMonitor::setFeedbackInterval(uint interval, uint8_t soundidx) {
  BeepInterval[soundidx] = interval;
}


void AudioMonitor::volumeUp() {
  Volume *= 1.414213;
  if (Volume > 0.8)
    Volume = 0.8;
  setVolume();
}


void AudioMonitor::volumeDown() {
  Volume /= 1.414213;
  if (Volume < 1/1024)
    Volume = 1/1024;
  setVolume();
}


void AudioMonitor::setVolume() {
  if (Play) {
    for (size_t i=0; i<4; i++)
      Mixer->gain(i, Gains[i]*Volume);
  }
}


void AudioMonitor::update() {
  if (!Play)
    return;
  if (VolumeButtons) {
    VolumeUpButton.update();
    VolumeDownButton.update();
    if (VolumeUpButton.pressed())
      volumeUp();
    if (VolumeDownButton.pressed())
      volumeDown();
  }
  for (size_t i=0; i<3; i++) {
    if (Beep[i] != 0 && BeepInterval[i] > 0 && BeepTime[i] > BeepInterval[i]) {
      BeepTime[i] = 0;
      Sound[i]->play((const unsigned int *)Beep[i]);
    }
  }
}




