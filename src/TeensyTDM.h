/*
  TeensyTDM - Streaming TDM data into a single cyclic buffer.
  Created by Jan Benda, July 4th, 2023.
  Heavily based on input_tdm.h of the Teensy Audio library.

  The Teensy is the master producing the MCLK, BCLK, and LRCLK.

  Based on input_tdm.h and input_tdm.cpp of the [Teensy Audio
  library](https://github.com/PaulStoffregen/Audio) and the
  setI2SFreq() function introduced by Frank B on the [Teensy
  forum](https://forum.pjrc.com/threads/38753-Discussion-about-a-simple-way-to-change-the-sample-rate/page4),
  see also [Frank's bat
  detector](https://forum.pjrc.com/threads/38988-Bat-detector) and the
  [Teensy
  Batdetector](https://github.com/CorBer/teensy_batdetector/releases/tag/v1.6).
*/

#ifndef TeensyTDM_h
#define TeensyTDM_h


#include <Arduino.h>
#include <ADC.h>
#include <DMAChannel.h>
#include <DataBuffer.h>


class TeensyTDM : public DataBuffer {

 public:
  
  TeensyTDM(volatile sample_t *buffer, size_t nbuffer);

  static TeensyTDM *TDM;
  
  // Set resolution of data slots to bits per sample.
  virtual void setResolution(uint8_t bits);
  
  // Set number of channels to nchannels.
  virtual void setNChannels(uint8_t nchannels);
  
  // The string identifying channel pins.
  const char *channels() { return Channels; };
  
  // Set string identifying channel pins.
  void setChannels(const char *cs);

  // Swap left/right channels.
  bool swapLR() const;

  // Set whether to swap left/right channels.
  void setSwapLR(bool swap=true);
  
  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on Serial.
  // If successfull, you may remove this check from your code.
  bool check();

  // Print current settings on Serial.
  void report();
 
  // Add metadata to the header of a wave file holding the data of the
  // buffer.
  virtual void setWaveHeader(WaveHeader &wave) const;

  // Start generation of clock signals.
  // Need to setup resolution and sampling rate before.
  void begin();

  // Start data transfer to buffer.
  void start();

  // Stop data transfer to buffer.
  void stop();

  
 protected:
  
  static DMAChannel DMA;

  void TDMISR();
  static void ISR();

  bool SwapLR;

  char Channels[128];
  
};


#endif
