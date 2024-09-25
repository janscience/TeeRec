#include <Arduino.h>
#include <DataBuffer.h>
#include <DataWorker.h>


#define DEBUG 1


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
  size_t head = 0;
  noInterrupts();
  head = Index;
  interrupts();
  return head;
}


size_t DataWorker::cycle() const {
  size_t headcycle = 0;
  noInterrupts();
  headcycle = Cycle;
  interrupts();
  return headcycle;
}


size_t DataWorker::available() const {
  if (Producer == 0 || Data == 0)
    return 0;
  size_t index = 0;
  size_t cycle = 0;
  noInterrupts();
  index = Producer->Index;
  cycle = Producer->Cycle;
  interrupts();
  if (cycle == Cycle && index > Index)
    return index - Index;
  else if (cycle == Cycle + 1 && index <= Index)
    return Data->nbuffer() - Index + index;
  else {
#ifdef DEBUG
    Serial.println("No data available in DataWorker:");
    Serial.print("    Worker cycle: ");
    Serial.print(Cycle);
    Serial.print(",   Worker index: ");
    Serial.println(Index);
    Serial.print("  Producer cycle: ");
    Serial.print(cycle);
    Serial.print(", Producer index: ");
    Serial.print(index);
    Serial.print(", Buffer size: ");
    Serial.println(Data->nbuffer());
#endif
    return 0;
  }
}


size_t DataWorker::overrun() {
  if (Producer == 0 || Data == 0)
    return 0;
  // get head:
  size_t index = 0;
  size_t cycle = 0;
  noInterrupts();
  index = Producer->Index;
  cycle = Producer->Cycle;
  interrupts();
  // compute number of missed samples:
  size_t missed = 0;
  if (cycle > Cycle+1 && index < Index) {
    missed = Data->nbuffer() - Index + index + (cycle-Cycle-2)*Data->nbuffer();
#ifdef DEBUG
    Serial.print("Overrun 1 in DataWorker by ");
    Serial.print(missed);
    Serial.println(" samples:");
    Serial.print("    Worker cycle: ");
    Serial.print(Cycle);
    Serial.print(",   Worker index: ");
    Serial.println(Index);
    Serial.print("  Producer cycle: ");
    Serial.print(cycle);
    Serial.print(", Producer index: ");
    Serial.print(index);
    Serial.print(", Buffer size: ");
    Serial.println(Data->nbuffer());
#endif
  }
  else if (cycle > Cycle && index >= Index) {
    missed = index - Index + (cycle-Cycle-1)*Data->nbuffer();
#ifdef DEBUG
    if (missed > 0) {
      Serial.print("Overrun 2 in DataWorker by ");
      Serial.print(missed);
      Serial.println(" samples:");
      Serial.print("    Worker cycle: ");
      Serial.print(Cycle);
      Serial.print(",   Worker index: ");
      Serial.println(Index);
      Serial.print("  Producer cycle: ");
      Serial.print(cycle);
      Serial.print(", Producer index: ");
      Serial.print(index);
      Serial.print(", Buffer size: ");
      Serial.println(Data->nbuffer());
    }
#endif
  }
  if (missed > 0) {
    // update tail:
    Index = index;
    Cycle = cycle - 1;
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


void DataWorker::synchronize(const DataWorker &worker) {
  Index = worker.Index;
  Cycle = worker.Cycle;
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
