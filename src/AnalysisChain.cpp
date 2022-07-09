#include <DataBuffer.h>
#include <Analyzer.h>
#include <AnalysisChain.h>


AnalysisChain::AnalysisChain(const DataWorker &data)
  : DataWorker(&data),
    NAnalyzer(0),
    Interval(1000),
    Window(0.1),
    Continuous(false),
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
  Counter = -1;
  Continuous = (fabs(interval - window) < 1e-8);
  NChannels = Data->nchannels();
  if (NChannels > MaxChannels)
    NChannels = MaxChannels;
  NFrames = Data->frames(Window);
  for(uint8_t c=0; c<NChannels; ++c) {
    Buffer[c] = (sample_t*)malloc(NFrames*sizeof(sample_t));
    if (Buffer[c] == 0) {
      Serial.println("Not enough memory to allocate analysis buffer!");
      while (1) {};
    }
  }
  synchronize();
  for (int i=0; i<NAnalyzer; i++) {
    if (Analyzers[i]->enabled()) {
      Analyzers[i]->setContinuous(Continuous);
      Analyzers[i]->setRate(Data->rate());
      Analyzers[i]->start(NChannels, NFrames);
    }
  }
}


void AnalysisChain::stop() {
  for (int i=0; i<NAnalyzer; i++) {
    if (Analyzers[i]->enabled())
      Analyzers[i]->stop();
  }
  for(uint8_t c=0; c<NChannels; ++c)
    free(Buffer[c]);
  NChannels = 0;
  NFrames = 0;
}


void AnalysisChain::update() {
  if (NChannels == 0 || NFrames == 0)
    return;
  if (Counter >= 0 ) {
    while ((Counter < NAnalyzer) && !Analyzers[Counter]->enabled())
      Counter++;
    if (Counter < NAnalyzer)
      Analyzers[Counter++]->analyze(Buffer, NChannels, NFrames);
    if (Counter >= NAnalyzer)
      Counter = -1;
  }
  else {
    if (Continuous) {
      if (available() < NChannels*NFrames)
	return;
    }
    else {
      if (Time <= Interval)
	return;
      Time -= Interval;
    }
    // get data:
    size_t start;
    if (Continuous)
      start = index();
    else
      start = Data->currentSample(NFrames);
    for (uint8_t c=0; c<NChannels; c++)
      Data->getData(c, start, Buffer[c], NFrames);
    if (Continuous)
      increment(NChannels * NFrames);
    else
      synchronize();
    Counter = 0;
  }
}
