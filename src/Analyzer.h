/*
  Analyzer - Coordinate analysis of data snippets.
  Created by Jan Benda, June 18th, 2022.
*/

#ifndef Analyzer_h
#define Analyzer_h


#include <Arduino.h>
#include <DataBuffer.h>


// Signalture of an analysis function.
// Note that the function is allowed to modify the data in place.
typedef void AnalysisFunc(float **data, int nchannels, int nsamples,
			  float rate);


class Analyzer {

 public:

  // Construct analyzer working on data.
  Analyzer(const DataBuffer *data);
  ~Analyzer();

  // Add a function for analysis.
  void addAnalyzer(AnalysisFunc *func);

  // Initialize analysis. Needs to be called before update() is used.
  void start();

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

  float **Buffer;
  uint8_t NChannels;
  size_t NSamples;
  float Rate;
  
};


#endif
