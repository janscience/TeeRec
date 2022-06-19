#include <Analyzer.h>
#include <AnalysisChain.h>


AnalysisChain::AnalysisChain(const DataBuffer &data)
  : Data(&data),
    NAnalyzer(0),
    Interval(1000),
    Window(0.1),
    Counter(-1),
    NChannels(0),
    NFrames(0)
{
  Time = 0;
}


AnalysisChain::~AnalysisChain() {
  stop();
}


void AnalysisChain::add(Analyzer &analyzer) {
  Analyzers[NAnalyzer++] = &analyzer;
}


void AnalysisChain::start(float interval, float window) {
  stop();
  Interval = 1000*interval;
  Window = window;
  Time = 0;
  NChannels = Data->nchannels();
  NFrames = Data->frames(Window);
  for(uint8_t c=0; c<NChannels; ++c) {
    Buffer[c] = (float*)malloc(NFrames*sizeof(float));
    if (Buffer[c] == 0) {
      Serial.println("Not enough memory to allocate analysis buffer!");
      while (1) {};
    }
  }
  for (int i=0; i<NAnalyzer; i++) {
    Analyzers[i]->setRate(Data->rate());
    Analyzers[i]->start();
  }
}


void AnalysisChain::stop() {
  for (int i=0; i<NAnalyzer; i++)
    Analyzers[i]->stop();
  for(uint8_t c=0; c<NChannels; ++c)
    free(Buffer[c]);
  NChannels = 0;
  NFrames = 0;
}


void AnalysisChain::update() {
  if (NChannels > 0 && NFrames > 0 && Time > Interval) {
    if (Counter < 0) {
      // get data:
      size_t start = Data->currentSample(NFrames);
      for (uint8_t c=0; c<NChannels; c++)
	Data->getData(c, start, Buffer[c], NFrames);
      Counter++;
    }
    else if (Counter < NAnalyzer)
      Analyzers[Counter++]->analyze(Buffer, NChannels, NFrames);
    if (Counter >= NAnalyzer) {
      Counter = -1;
      Time -= Interval;
    }
  }
}
