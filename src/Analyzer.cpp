#include <AnalysisChain.h>
#include <Analyzer.h>


Analyzer::Analyzer(AnalysisChain *chain) :
  Enabled(true),
  Rate(0) {
  if (chain != 0)
    chain->add(*this);
}


void Analyzer::setRate(float rate) {
  Rate = rate;
}


bool Analyzer::enabled() const {
  return Enabled;
}


void Analyzer::enable() {
  Enabled = true;
}


void Analyzer::disable() {
  Enabled = false;
}


void Analyzer::start() {
}


void Analyzer::stop() {
}


