/*
  DataBuffer - A single cyclic, multiplexed buffer holding acquired data.
  Created by Jan Benda, July 6th, 2021.
*/

#ifndef DataBuffer_h
#define DataBuffer_h


#include <Arduino.h>
#include <DataWorker.h>
#include <TeensyBoard.h>


typedef int16_t sample_t;


// Macro for defining the one and only data buffer.
// buffer and nbuffer are the variable names for the buffer and its size.
// n defines the number of samples the buffer can hold.
// DANGER: The buffer size must be a multiple of InputADC::MajorSize (256)
// and the maximum number of channels per ADC (8)!
// For Teensy 3.2 use n = 256*32 (16kB),
// for Teensy 3.5/3.6 use n = 256*256 (128kB) or less.
// for Teensy 4 you may use at least n = 512*256 (256kB).
#define DATA_BUFFER(buffer, nbuffer, n) \
  static const size_t nbuffer = n;				   \
  static volatile sample_t __attribute__((aligned(32))) buffer[n]; \

// Same as DATA_BUFFER but allocates the buffer in PSRAM (Teensy 4.1 only).
#define EXT_DATA_BUFFER(buffer, nbuffer, n) \
  static const size_t nbuffer = n;				   \
  static volatile EXTMEM sample_t __attribute__((aligned(32))) buffer[n]; \


class DataBuffer : public DataWorker {
  
public:

  // Pass a buffer that has been created with the DATA_BUFFER macro.
  DataBuffer(volatile sample_t *buffer, size_t nbuffer, size_t dmabuffer=0);
  
  // Return total number of samples the buffer holds.
  size_t nbuffer() const { return NBuffer; };
 
  // Return the buffer.
  volatile sample_t *buffer() const { return Buffer; };

  // Return resolution at data acquisition in bits per sample.
  uint8_t resolution() const { return Bits; };
  
  // Set resolution of data acquisition to bits per sample.
  virtual void setResolution(uint8_t bits);

  // Return used resolution of data buffer in bits per sample (max 16 bits).
  uint8_t dataResolution() const { return DataBits; };

  // Set used resolution of data buffer in bits per sample (max 16 bits).
  virtual void setDataResolution(uint8_t bits);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate; };
  
  // Set sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return total number of channels multiplexed into the buffer.
  uint8_t nchannels() const { return NChannels; };
  
  // Set number of channels to nchannels.
  virtual void setNChannels(uint8_t nchannels);

  // Time the cyclic buffer can hold in seconds given the
  // sampling rate and the number of channels.
  float bufferTime() const;

  // The size of a DMA buffer in samples.
  size_t DMABufferSize() const { return NDMABuffer; };

  // Time the DMA buffer can hold in seconds given the
  // sampling rate and the number of channels.
  float DMABufferTime() const;
  
  // Inform the DataBuffer about the size of a DMA buffer used to get the data.
  void setDMABufferSize(size_t samples);

  // Get nframes data from a channel starting at sample index start.
  // Assumes start to be the first index of a frame, not the one of the channel.
  void getData(uint8_t channel, size_t start,
	       sample_t *buffer, size_t nframes) const;

  // Get the nframes most recent data from a channel scaled to (-1, 1).
  // Assumes start to be the first index of a frame, not the one of the channel.
  void getData(uint8_t channel, size_t start,
	       float *buffer, size_t nframes) const;

  // Print nframes samples of all channels starting at sample start.
  // Each line is one frame with channels separated by ';'.
  void printData(size_t start, size_t nframes, Stream &stream=Serial) const;

  // Print nframes samples of the specified channel starting at sample start.
  void printData(uint8_t channel, size_t start, size_t nframes,
		 Stream &stream=Serial) const;

  // Check whether data in the whole buffer are within the specified range
  // (for debugging).
  void checkData(int32_t min, int32_t max) const;

  
protected:

  size_t NBuffer;            // number of samples the buffer can hold.
  volatile sample_t *Buffer; // pointer to the one and only buffer
  uint8_t Bits;
  uint32_t Rate;             // sampling rate per channel
  uint8_t NChannels;         // number of channels multiplexed into the buffer
  uint8_t DataBits;
  size_t NDMABuffer;         // number of samples of a DMA buffer
  
};


#endif
