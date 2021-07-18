/*
  DataBuffer - A single cyclic, multiplexed buffer holding acquired data.
  Created by Jan Benda, July 6th, 2021.
*/

#ifndef DataBuffer_h
#define DataBuffer_h


#include <Arduino.h>
#include <TeensyBoard.h>


typedef int16_t sample_t;


class DataConsumer;


class DataBuffer {


  friend class DataConsumer;
  
public:

  DataBuffer();

  // add consumer to data.
  void addConsumer(DataConsumer *consumer) const;

  // reset the buffer and consumers.
  void reset();

  // Return total number of samples the buffer holds.
  size_t nbuffer() const { return NBuffer; };

  // Return the buffer.
  volatile sample_t *buffer() const { return Buffer; };

  // Return resolution of data buffer in bits per sample (always 16 bits).
  uint8_t dataResolution() const { return DataBits; };

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const { return Rate; };

  // Return total number of channels multiplexed into the buffer.
  uint8_t nchannels() const { return NChannels; };

  // Time the cyclic buffer can hold in seconds.
  float bufferTime() const;

  // Number of frames (samples of a single channel) corresponding to time (in seconds).
  size_t frames(float time) const;

  // Number of samples (samples of all channel) corresponding to time (in seconds).
  size_t samples(float time) const;

  // Time in seconds corresponding to a given number of samples (not frames).
  float time(size_t samples) const;

  // Return time corresponding to samples as a string displaying minutes and seconds.
  // str must hold at least 6 characters.
  void timeStr(size_t sample, char *str) const;

  // Return current value of the head index.
  size_t head() const;

  // Return current value of the head cycle counter.
  size_t headCycle() const;

  // Return sample right after most current data value in data buffer
  // optionally decremented by decr frames.
  size_t currentSample(size_t decr=0);

  // Decrement sample index into data buffer by decr times number of channels.
  size_t decrementSample(size_t idx, size_t decr);

  // Increment sample index into data buffer by decr times number of channels.
  size_t incrementSample(size_t idx, size_t incr);

  // Get the nbuffer most recent data from a channel scaled to (-1, 1). <1ms
  void getData(uint8_t channel, size_t start, float *buffer, size_t nbuffer);

  // Check whether data in the whole buffer are within the specified range (for debugging).
  void checkData(int32_t min, int32_t max);

  
protected:

  // DANGER: Buffer size must be a multiple of ContinuousADC::MajorSize and the maximum number of channels per ADC (16)!
 #ifdef TEENSY32
  static const size_t NBuffer = 256*32;     // buffer size: 16kB
#else
  static const size_t NBuffer = 256*256;    // buffer size: 128kB
#endif
  volatile static sample_t __attribute__((aligned(32))) Buffer[NBuffer]; // the one and only buffer
  volatile size_t Head;                     // current index of latest data
  volatile size_t HeadCycle;                // count buffer cycles
  uint32_t Rate;                            // sampling rate per channel
  uint8_t NChannels;                        // number of channels multiplexed into the buffer
  static const uint8_t DataBits = 16;
  
  static const size_t MaxConsumers = 10;
  mutable size_t NConsumers;
  mutable DataConsumer *Consumers[MaxConsumers];
};


// Base class for all objects operating on the data buffer.
class DataConsumer {
  
public:

  DataConsumer();
  DataConsumer(const DataBuffer *data);

  void setData(const DataBuffer *data);

  // Reset consumer.
  virtual void reset();

  // Number of samples available for consumption.
  size_t available() const;

  // Number of samples that have been missed to be consumed.
  // Sets the tail forward to the first still available sample.
  size_t overrun();


protected:
  
  const DataBuffer *Data;
  size_t Tail;       // index for reading the buffer.
  size_t TailCycle;  // count buffer cycles.
  
};


#endif
