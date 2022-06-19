#include <AnalysisChain.h>
#include <Analyzer.h>


Analyzer::Analyzer(AnalysisChain *chain) :
  Rate(0) {
  if (chain != 0)
    chain->add(*this);
}


void Analyzer::setRate(float rate) {
  Rate = rate;
}


void Analyzer::start() {
}


void Analyzer::stop() {
}


