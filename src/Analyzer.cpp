#include <Analyzer.h>


Analyzer::Analyzer(const DataBuffer *data)
  : Data(data),
    NFuncs(0),
    Interval(1000),
    Window(0.1),
    Counter(-1),
    Buffer(0),
    NChannels(0),
    NSamples(0),
    Rate(0)
{
  Time = 0;
}


void Analyzer::addAnalyzer(AnalysisFunc *func) {
  Analyze[NFuncs++] = func;
}


void Analyzer::start() {
  Time = 0;
  NChannels = Data->nchannels();
  NSamples = Data->frames(Window);
  Buffer = new float*[NChannels];
  for(uint8_t i=0; i<NChannels; ++i)
    Buffer[i] = new float[NSamples];
  Rate = Data->rate();
}


void Analyzer::stop() {
  for(uint8_t i=0; i<Data->nchannels(); ++i)
    delete [] Buffer[i];
  delete [] Buffer;
  NChannels = 0;
  NSamples = 0;
}


void Analyzer::update() {
  if (NChannels > 0 && NSamples > 0 && Time > Interval) {
    if (Counter < 0) {
      // get data:
      size_t start = Data->currentSample(NSamples);
      for (uint c=0; c<NChannels; c++)
	Data->getData(c, start, Buffer[c], NSamples);
    }
    else if (Counter < NFuncs)
      Analyze[Counter++](Buffer, NChannels, NSamples, Rate);
    if (Counter >= NFuncs) {
      Counter = -1;
      Time -= Interval;
    }
  }
}
