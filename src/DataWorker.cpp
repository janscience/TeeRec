#include <Arduino.h>
#include <DataBuffer.h>
#include <DataWorker.h>


DataWorker::DataWorker(int verbose) :
  Index(0),
  Cycle(0),
  Data(0),
  Producer(0),
  NConsumers(0),
  Verbose(verbose),
  Gain(1.0),
  PreGain(1.0),
  Unit("") {
}


DataWorker::DataWorker(const DataWorker *producer, int verbose) :
  DataWorker(verbose) {
  setProducer(producer);
}


void DataWorker::setProducer(const DataWorker *producer) {
  producer->addConsumer(this);
  Producer = producer;
  Data = producer->Data;
  Gain = producer->Gain;
  PreGain = producer->PreGain;
  strcpy(Unit, producer->Unit);
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
    if (Verbose > 3) {
      float dt = NoDataTime;
      Serial.printf("  No new data available in DataWorker (previous warning %.3fms ago):\n", 0.001*dt);
      if (Cycle != cycle || Index != index) {
	Serial.print("      Worker cycle: ");
	Serial.print(Cycle);
	Serial.print(",     Worker index: ");
	Serial.println(Index);
	Serial.print("    Producer cycle: ");
	Serial.print(cycle);
	Serial.print(",   Producer index: ");
	Serial.print(index);
	Serial.print(", Buffer size: ");
	Serial.println(Data->nbuffer());
      }
      NoDataTime = 0;
    }
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
    if (Verbose > 2) {
      Serial.print("ERROR in DataWorker::overrun(): overrun 1 by ");
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
  }
  else if (cycle > Cycle && index >= Index) {
    missed = index - Index + (cycle-Cycle-1)*Data->nbuffer();
    if (missed > 0 && Verbose > 2) {
      Serial.print("ERROR in DataWorker::overrun(): overrun 2 by ");
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
  }
  if (missed > 0) {
    // update tail:
    Index = index;
    Cycle = cycle - 1;
  }
  return missed;
}


void DataWorker::setVerbosity(int verbose) {
  Verbose = verbose;
}


void DataWorker::setGain(float gain) {
  Gain = gain;
  for (size_t k=0; k<NConsumers; k++)
    Consumers[k]->setGain(gain);
}


void DataWorker::setPreGain(float pregain) {
  PreGain = pregain;
  for (size_t k=0; k<NConsumers; k++)
    Consumers[k]->setPreGain(pregain);
}


void DataWorker::setUnit(const char *unit) {
  strncpy(Unit, unit, MaxUnit);
  Unit[MaxUnit - 1] = '\0';
  for (size_t k=0; k<NConsumers; k++)
    Consumers[k]->setUnit(unit);
}


void DataWorker::gainStr(char *gains) {
  sprintf(gains, "%.2f%s", gain(), unit());
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
