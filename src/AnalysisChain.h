/*
  AnalysisChain - Coordinate analysis of data snippets.
  Created by Jan Benda, June 18th, 2022.
*/

#ifndef AnalysisChain_h
#define AnalysisChain_h


#include <Arduino.h>
#include <DataBuffer.h>


// Signalture of an analysis function.
// Note that the function is allowed to modify the data in place.
typedef void AnalysisFunc(float **data, uint8_t nchannels, size_t nframes,
			  float rate);


class AnalysisChain {

 public:

  // Construct analyzer working on data.
  AnalysisChain(const DataBuffer &data);
  ~AnalysisChain();

  // Add a function for analysis.
  void add(AnalysisFunc *func);

  // Initialize analysis. Needs to be called before update() is used.
  // Analysis functions will be called every interval seconds on a data window
  // of window seconds length.
  void start(float interval, float window);

  // Clean up analysis. After this update() will ot do anything.
  void stop();

  // Call the analysis functions at appropriate times.
  // Call this functions as often as possible in loop().
  void update();
  
  
 protected:

  const DataBuffer *Data;    // XXX shouldn't that be a DataWorker?!!
  
  static const int MaxFuncs = 10;
  int NFuncs;
  AnalysisFunc *Analyze[MaxFuncs];

  uint Interval;
  float Window;
  elapsedMillis Time;
  int Counter;

  float *Buffer[16];
  uint8_t NChannels;
  size_t NFrames;
  float Rate;
  
};


#endif
