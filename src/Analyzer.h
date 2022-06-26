/*
  Analyzer - Base class for analyzer called by AnalysisChain.
  Created by Jan Benda, June 19th, 2022.
*/

#ifndef Analyzer_h
#define Analyzer_h


#include <Arduino.h>


class AnalysisChain;


class Analyzer {

 public:

  // Construct analyzer and add it to an AnalysisChain.
  Analyzer(AnalysisChain *chain=0);

  // True if this analyzer is enabled.
  bool enabled() const;

  // Enable this analyzer.
  void enable();

  // Disable this analyzer.
  void disable();

  // True if the data provided to analyze() are continuous.
  bool continuous() const;

  // Set sampling rate of data. This is done by AnalysisChain::start().
  void setContinuous(bool continuous);

  // Set sampling rate of data. This is done by AnalysisChain::start().
  void setRate(float rate);

  // Start and initialize analyzer. Default implementation does nothing.
  virtual void start(uint8_t nchannels, size_t nframes);

  // Stop analyzer. Default implementation does nothing.
  virtual void stop();

  // Analyze data of nchannels channels each holding nframes frames of data.
  // Note that this function is allowed to modify the data in place.
  virtual void analyze(float **data, uint8_t nchannels, size_t nframes) = 0;
  
  
 protected:

  bool Enabled;
  bool Continuous;
  float Rate;
  
};


#endif
