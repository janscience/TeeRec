/*
  AnalysisChain - Coordinate analysis of data snippets via Analyzer.
  Created by Jan Benda, June 18th, 2022.
*/

#ifndef AnalysisChain_h
#define AnalysisChain_h


#include <Arduino.h>
#include <DataBuffer.h>


class Analyzer;


class AnalysisChain {

 public:

  // Construct analysis chain working on data.
  AnalysisChain(const DataBuffer &data);
  ~AnalysisChain();

  // Add an analyzer to analysis chain.
  void add(Analyzer &analyzer);

  // Initialize analysis. Needs to be called before update() is used.
  // Analysis functions will be called every interval seconds on a data window
  // of window seconds length.
  void start(float interval, float window);

  // Clean up analysis. After this update() will not do anything.
  void stop();

  // Call the analysis functions at appropriate times.
  // Call this functions as often as possible in loop().
  void update();
  
  
 protected:

  const DataBuffer *Data;    // XXX shouldn't that be a DataWorker?!!
  
  static const int MaxAnalyzer = 10;
  int NAnalyzer;
  Analyzer *Analyzers[MaxAnalyzer];

  uint Interval;
  float Window;
  elapsedMillis Time;
  int Counter;

  float *Buffer[16];
  uint8_t NChannels;
  size_t NFrames;
  
};


#endif
