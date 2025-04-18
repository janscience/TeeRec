/*
  InputTDM - Streaming TDM data into a single cyclic buffer.
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

#ifndef InputTDM_h
#define InputTDM_h


#include <Arduino.h>
#include <ADC.h>
#include <DMAChannel.h>
#include <Input.h>


class InputTDM : public Input {

 public:
  
  enum TDM_BUS {
    TDM1=0,     // Teensy 3: pin 13, Teensy 4: pin 8
#if defined(__IMXRT1062__)
    TDM2=1      // Teensy 4: pin 5
#endif
  };
  
  InputTDM(volatile sample_t *buffer, size_t nbuffer);

  static InputTDM *TDM;
  
  // Set resolution of data slots to bits per sample.
  virtual void setResolution(uint8_t bits);

  // When transfering data from the TDM data stream to the buffer,
  // downsample them by n samples.
  void downSample(uint8_t n);

  // Return total number of channels multiplexed into the buffer.
  uint8_t nchannels() const { return NChannels; };
  
  // Set number of channels to nchannels.
  virtual void setNChannels(uint8_t nchannels);
  
  // Set number of channels of the TDM bus to nchannels.
  void setNChannels(TDM_BUS bus, uint8_t nchannels);

  // Return number of channels recorded from the given TDM bus.
  uint8_t nchannels(TDM_BUS bus) const { return NChans[bus]; };
  
  // Return in chans of size nchans the string with the channels
  // in the order they are multiplexed into the buffer.
  virtual void channelsStr(char *chans, size_t nchans) const;
  
  // Set string identifying channel pins.
  // Len of cs must be smaller than MaxChannels bytes.
  void setChannelsStr(const char *cs);
  
  // Clear the channels for a given TDM bus.
  void clearChannels(TDM_BUS bus);
  
  // Clear the channel configuration of all TDM buses.
  virtual void clearChannels();

  // Swap left/right channels.
  bool swapLR() const;

  // Set whether to swap left/right channels.
  void setSwapLR(bool swap=true);

  // Return DMA counter for specified TDM bus.
  size_t counter(TDM_BUS bus) const;
  
  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on Serial.
  // If nchannels is greater than zero, check will fail if
  // the number of configured channels does not match nchannels.
  // If successfull, you may remove this check from your code.
  virtual bool check(uint8_t nchannels=0, Stream &stream=Serial);

  // Print current settings on Serial.
  virtual void report(Stream &stream=Serial);
 
  // Add metadata to the header of a wave file holding the data of the
  // buffer.
  virtual void setWaveHeader(WaveHeader &wave) const;

  // Start generation of clock signals.
  // Need to setup resolution and sampling rate before.
  virtual void begin(Stream &stream=Serial);

  // Start data transfer to buffer.
  virtual void start();

  // Stop data transfer to buffer.
  virtual void stop();

  // Maximum size of channel descriptor string.
  static const size_t MaxChannels = 256;

  
protected:
  
  static DMAChannel DMA[2];
  volatile size_t DMACounter[2];  // total count of TDMBuffer segments
  volatile size_t DataHead[2];    // current index for each TDM bus for writing. Only used in isr.

  void TDMISR(uint8_t tdm);
  static void ISR0();
#if defined(__IMXRT1062__)
  static void ISR1();
#endif
  
  uint8_t DownSample;
  bool SwapLR;

  char Channels[MaxChannels];

  uint8_t TDMUse;
  uint8_t NChans[2];
  
};


#endif
