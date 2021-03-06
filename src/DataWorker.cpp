#include <Arduino.h>
#include <DataBuffer.h>
#include <DataWorker.h>


DataWorker::DataWorker() {
  Index = 0;
  Cycle = 0;
  Data = 0;
  Producer = 0;
  NConsumers = 0;
}


DataWorker::DataWorker(const DataWorker *producer) {
  Index = 0;
  Cycle = 0;
  setProducer(producer);
  NConsumers = 0;
}


void DataWorker::setProducer(const DataWorker *producer) {
  producer->addConsumer(this);
  Producer = producer;
  Data = producer->Data;
}


void DataWorker::addConsumer(DataWorker *consumer) const {
  Consumers[NConsumers++] = consumer;
}


void DataWorker::reset() {
  Index = 0;
  Cycle = 0;
  for (size_t k=0; k<NConsumers; k++)
    Consumers[k]->reset();
}


size_t DataWorker::index() const {
  unsigned char sreg_backup;
  size_t head = 0;
  sreg_backup = SREG;
  cli();
  head = Index;
  SREG = sreg_backup;
  return head;
}


size_t DataWorker::cycle() const {
  unsigned char sreg_backup;
  size_t headcycle = 0;
  sreg_backup = SREG;
  cli();
  headcycle = Cycle;
  SREG = sreg_backup;
  return headcycle;
}


size_t DataWorker::available() const {
  if (Producer == 0 && Data == 0)
    return 0;
  size_t index = 0;
  size_t cycle = 0;
  unsigned char sreg_backup = SREG;
  cli();
  if (Producer != 0) {
    index = Producer->Index;
    cycle = Producer->Cycle;
  }
  else {
    index = Data->Index;
    cycle = Data->Cycle;
  }
  SREG = sreg_backup;
  if (Index < index)
    return index - Index;
  else if (Cycle < cycle)
    return Data->nbuffer() - Index + index;
  else
    return 0;
}


size_t DataWorker::overrun() {
  if (Producer == 0 || Data == 0)
    return 0;
  // get head:
  size_t index = 0;
  size_t cycle = 0;
  unsigned char sreg_backup = SREG;
  cli();
  index = Producer->Index;
  cycle = Producer->Cycle;
  SREG = sreg_backup;
  // compute number of missed samples:
  size_t missed = 0;
  if (index >= Index && cycle > Cycle)
    missed = index - Index + (cycle-Cycle-1)*Data->nbuffer();
  else if (index < Index && cycle > Cycle+1)
    missed = Data->nbuffer() - Index + index + (cycle-Cycle-2)*Data->nbuffer();
  if (missed > 0) {
    // update tail:
    Index = index + Data->nchannels();
    Cycle = cycle - 1;
    if (Index >= Data->nbuffer()) {
      Index -= Data->nbuffer();
      Cycle++;
    }
  }
  return missed;
}


void DataWorker::setWaveHeader(WaveHeader &wave) const {
  if (Producer != 0)
    Producer->setWaveHeader(wave);
}


bool DataWorker::synchronize() {
  if (Producer == 0) {
    reset();
    return false;
  }
  Index = Producer->Index;
  Cycle = Producer->Cycle;
  return true;
}


bool DataWorker::decrement(size_t indices) {
  if (indices > Data->nbuffer())
    indices = Data->nbuffer();
  if (indices <= Index) {
    Index -= indices;
    return false;
  }
  else if (Cycle > 0) {
    Index += Data->nbuffer() - indices;
    Cycle--;
    return true;
  }
  else {
    Index = 0;
    return false;
  }
}


bool DataWorker::increment(size_t indices) {
  Index += indices;
  bool r = false;
  while (Index >= Data->nbuffer()) {
    Index -= Data->nbuffer();
    Cycle++;
    r = true;
  }
  return r;
}
