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

  
 protected:

  void setup(uint8_t nchannels, uint8_t bits, uint32_t rate);
  void setupTDM();
  void setupDMA();
  
  void TDMISR();
  static void ISR();
  
  static DMAChannel DMA;
  
};


#endif
