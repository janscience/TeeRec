#include <Analyzer.h>


Analyzer::Analyzer(const DataBuffer &data)
  : Data(&data),
    NFuncs(0),
    Interval(1000),
    Window(0.1),
    Counter(-1),
    NChannels(0),
    NFrames(0),
    Rate(0)
{
  Time = 0;
}


Analyzer::~Analyzer() {
  stop();
}


void Analyzer::add(AnalysisFunc *func) {
  Analyze[NFuncs++] = func;
}


void Analyzer::start(float interval, float window) {
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
  Rate = Data->rate();
}


void Analyzer::stop() {
  for(uint8_t c=0; c<NChannels; ++c)
    free(Buffer[c]);
  NChannels = 0;
  NFrames = 0;
}


void Analyzer::update() {
  if (NChannels > 0 && NFrames > 0 && Time > Interval) {
    if (Counter < 0) {
      // get data:
      size_t start = Data->currentSample(NFrames);
      for (uint8_t c=0; c<NChannels; c++)
	Data->getData(c, start, Buffer[c], NFrames);
      Counter++;
    }
    else if (Counter < NFuncs)
      Analyze[Counter++](Buffer, NChannels, NFrames, Rate);
    if (Counter >= NFuncs) {
      Counter = -1;
      Time -= Interval;
    }
  }
}
