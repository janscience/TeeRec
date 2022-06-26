#include <AnalysisChain.h>
#include <Analyzer.h>


Analyzer::Analyzer(AnalysisChain *chain) :
  Enabled(true),
  Continuous(false),
  Rate(0) {
  if (chain != 0)
    chain->add(*this);
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


bool Analyzer::continuous() const {
  return Continuous;
}


void Analyzer::setContinuous(bool continuous) {
  Continuous = continuous;
}


void Analyzer::setRate(float rate) {
  Rate = rate;
}


void Analyzer::start() {
}


void Analyzer::stop() {
}


