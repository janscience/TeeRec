/*
  Input - Base class for all input streams into a single cyclic buffer.
  Created by Jan Benda, August 29th, 2023.
*/

#ifndef Input_h
#define Input_h


#include <Arduino.h>
#include <DataBuffer.h>


class Input : public DataBuffer {

 public:
  
  static const size_t MajorSize = 256;

  // Initialize and pass a buffer that has been created with the
  // DATA_BUFFER macro.
  Input(volatile sample_t *buffer, size_t nbuffer, size_t dmabuffer=0);

  // Return in chans a string with the channels/pins
  // in the order they are multiplexed into the buffer.
  virtual void channels(char *chans) const = 0;
  
  // Clear the channel configuration.
  virtual void clearChannels() = 0;

  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on stream.
  // Default implementation returns false if nchannels is greater than
  // zero and the number of configured channels does not match
  // nchannels.
  // If successfull, you may remove this check from your code.
  virtual bool check(uint8_t nchannels=0, Stream &stream=Serial);

  // Print current settings on stream.
  virtual void report(Stream &stream=Serial) = 0;
  
  // Prepare start of acquisition.
  // For example, start generating a clock signal for data transfer.
  // The default implementation does nothing.
  virtual void begin(Stream &stream=Serial);

  // Start the acquisition based on the channel, rate, and buffer settings.
  // Default implementation stores start time and sets Running to True.
  virtual void start();

  // True if data acquisition is running and transfering to buffer.
  bool running() const { return Running; };

  // The time when data acquisition was started in milliseconds.
  uint32_t startTime() const { return StartTime; };

  // Stop acquisition.
  // Default implementation sets Running to False.
  virtual void stop();


protected:
  
  bool Running;
  mutable uint32_t StartTime;
  
};


#endif
