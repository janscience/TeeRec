/*
  TeensyTDM - Streaming TDM data into a single cyclic buffer.
  Created by Jan Benda, July 4th, 2023.
  Heavily based on input_tdm.h of the Teensy Audio library.

  The Teensy is the master producing the MCLK, BCLK, and LRCLK.
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
  
};


#endif
