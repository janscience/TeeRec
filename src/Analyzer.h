/*
  Analyzer - Coordinate analysis of data snippets.
  Created by Jan Benda, June 18th, 2022.
*/

#ifndef Analyzer_h
#define Analyzer_h


#include <Arduino.h>
#include <DataBuffer.h>


typedef void AnalysisFunc(float **data, int nchannels, int nsamples,
			  float rate);


class Analyzer {

 public:

  Analyzer(const DataBuffer *data);

  void addAnalyzer(AnalysisFunc *func);

  void start();

  void stop();

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
