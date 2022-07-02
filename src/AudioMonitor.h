/*
  AudioMonitor - Play recorded data with optional feedback signals on speaker.
  Created by Jan Benda, May 25, 2022.
*/

#ifndef AudioMonitor_h
#define AudioMonitor_h


#include <Arduino.h>
#include <Audio.h>
#include <Bounce2.h>
#include <DataWorker.h>
#include <AudioPlayBuffer.h>


class AudioMonitor {
  
 public:

  // Construct and AudioMonitor. It plays the data from `data` on the
  // `speaker`. Up to three optional audio feddback signals are mixed
  // in as set up via `addFeddback()`.
  // Note: you need to allocate AudioMemory() for the Audio library yourself.
  AudioMonitor(DataWorker &data, AudioStream &speaker);

  // Setup an amplifier for the audio monitor.
  // If `amplifier_pin` is positive, it is configured for output and
  // set to high to switch on/enable an amplifier chip.
  // The amplifier pin is configured to `mode`.
  void setupAmp(int amplifier_pin, int mode=INPUT_PULLUP);

  // Setup volume control for the audio monitor.
  // The general volume is set to `volume` (between 0 and 1).
  // If both `volume_up_pin` and `volume_down_pin` are positive these
  // pins are configured to `mode` and are monitored for button
  // presses to regulate the volume.
  void setupVolume(float volume, int volume_up_pin=-1,
		   int volume_down_pin=-1, int mode=INPUT_PULLUP);
  
  // Set low-pass filter time-constant for audio output to n/44.1kHz,
  // default is 10. Must be larger or equal to one.
  void setLowpass(int16_t n);

  // Pause playing data and feedbacks on speaker.
  void pause();

  // Resume playing data and feedbacks on speaker.
  void play();

  // Add an audio feedback channel and initialize feedback sound with
  // frequency `freq` in Hertz and duration `duration` in seconds.
  // Of the maximum possible gain of one that has to accomodate the
  // actual monitor signal and all the feedback signals, this feedback
  // gets the relative gain of `gain`. The gain of the audio monitor
  // is reduced by `gain`.
  void addFeedback(float gain, float freq, float duration);

  // Set interval between audio feedback of feedback channel
  // `soundix` to `interval` milliseonds.
  void setFeedbackInterval(uint interval, uint8_t soundidx=0);

  // Set interval between audio feedback of feedback channel
  // `soundix` according to frac.
  // For frac equal to or larger than one, a feedback signal is played
  // with the smallest interval (5Hz).
  // The smaller frac, the larger the interval between the feedback signals.
  // If frac is les or equal to zero, no feedback signal is played.
  void setFeedback(float frac, uint8_t soundidx=0);

  // Call as often as possible in loop().
  // Checks volume buttons and calls volumeUp() and volumeDown() accordingly.
  // Initiates playing audio feedbacks.
  void update();

  // Scale the general Volume up by sqrt(2).
  // Called from update() whenever VolumeUpButton is pressed.
  void volumeUp();
  
  // Scale the general Volume down by sqrt(2).
  // Called from update() whenever VolumeDownButton is pressed.
  void volumeDown();
  

 protected:

  // Set volumes on the mixer from the relative Gains and the general Volume.
  void setVolume();

  AudioPlayBuffer Data;
  AudioPlayMemory *Sound[3];
  AudioMixer4 *Mixer;
  AudioConnection *ACI;
  AudioConnection *ACO;
  AudioConnection *ACS[3];
  bool Play;
  int AmplifierPin;  
  int NSounds;
  float Volume;
  float Gains[4];
  bool VolumeButtons;
  Button VolumeUpButton;
  Button VolumeDownButton;
  int16_t *Beep[3];
  int BeepDuration[3];
  uint BeepInterval[3];
  elapsedMillis BeepTime[3];

};


#endif
