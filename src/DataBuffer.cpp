#include <Arduino.h>
#include <DataBuffer.h>


volatile sample_t __attribute__((aligned(32))) DataBuffer::Buffer[NBuffer];


DataBuffer::DataBuffer() {
  Head = 0;
  HeadCycle = 0;
  Rate = 0;
  NChannels = 0;
  NConsumers = 0;
  memset((void *)Buffer, 0, sizeof(sample_t)*NBuffer);
}


void DataBuffer::addConsumer(DataConsumer *consumer) const {
  Consumers[NConsumers++] = consumer;
}


void DataBuffer::reset() {
  Head = 0;
  HeadCycle = 0;
  for (size_t k=0; k<NConsumers; k++)
    Consumers[k]->reset();
}


float DataBuffer::bufferTime() const {
  return float(NBuffer/NChannels)/Rate;
}


size_t DataBuffer::frames(float time) const {
  return floor(time*Rate);
}


size_t DataBuffer::samples(float time) const {
  return floor(time*Rate)*NChannels;
}


float DataBuffer::time(size_t samples) const {
  return float(samples/NChannels)/Rate;
}


void DataBuffer::timeStr(size_t samples, char *str) const {
  float seconds = time(samples);
  float minutes = floor(seconds/60.0);
  seconds -= minutes*60;
  sprintf(str, "%02.0f:%02.0f", minutes, seconds);
}


size_t DataBuffer::head() const {
  unsigned char sreg_backup;
  size_t head = 0;
  sreg_backup = SREG;
  cli();
  head = Head;
  SREG = sreg_backup;
  return head;
}


size_t DataBuffer::headCycle() const {
  unsigned char sreg_backup;
  size_t headcycle = 0;
  sreg_backup = SREG;
  cli();
  headcycle = HeadCycle;
  SREG = sreg_backup;
  return headcycle;
}


size_t DataBuffer::currentSample(size_t decr) {
  size_t idx = head();
  if (decr > 0) {
    idx += NBuffer - decr*NChannels;
    while (idx > NBuffer)
      idx -= NBuffer;
  }
  return idx;
}


size_t DataBuffer::decrementSample(size_t idx, size_t decr) {
  idx += NBuffer - decr*NChannels;
  while (idx > NBuffer)
    idx -= NBuffer;
  return idx;
}


size_t DataBuffer::incrementSample(size_t idx, size_t incr) {
  idx += incr*NChannels;
  while (idx > NBuffer)
    idx -= NBuffer;
  return idx;
}


void DataBuffer::getData(uint8_t channel, size_t start, float *buffer, size_t nbuffer) {
  if ( Rate == 0 || NChannels == 0 ) {
    memset(buffer, 0, sizeof(sample_t)*nbuffer);
    return;
  }
  if (nbuffer*NChannels > NBuffer) {
    Serial.println("ERROR: requested too many samples.");
    memset(buffer, 0, sizeof(sample_t)*nbuffer);
    return;
  }
  // copy:
  start += channel;
  float scale = 1.0/(1 << (DataBits-1));
  for (size_t k=0; k<nbuffer; k++ ) {
    if (start >= NBuffer)
      start -= NBuffer;
    buffer[k] = scale*Buffer[start];
    start += NChannels;
  }
}


void DataBuffer::checkData(int32_t min, int32_t max) {
  for (unsigned int k=0; k<NBuffer; k++) {
    sample_t data = Buffer[k];
    if (data < min)
      Serial.printf("%d: %d < %d\n", k, data, min);
    else if (data > max)
      Serial.printf("%d: %d > %d\n", k, data, max);
  }
}


DataConsumer::DataConsumer() {
  Tail = 0;
  Data = 0;
}


DataConsumer::DataConsumer(const DataBuffer *data) {
  Tail = 0;
  setData(data);
}


void DataConsumer::setData(const DataBuffer *data) {
  data->addConsumer(this);
  Data = data;
}


void DataConsumer::reset() {
  Tail = 0;
}


size_t DataConsumer::available() const {
  if (Data == NULL)
    return 0;
  size_t head = 0;
  unsigned char sreg_backup = SREG;
  cli();
  head = Data->Head;
  SREG = sreg_backup;
  if (Tail <= head)
    return head - Tail;
  else
    return Data->NBuffer - Tail + head;
}


size_t DataConsumer::overrun() {
  if (Data == NULL)
    return 0;
  // get head:
  size_t head = 0;
  size_t headcycle = 0;
  unsigned char sreg_backup = SREG;
  cli();
  head = Data->Head;
  headcycle = Data->HeadCycle;
  SREG = sreg_backup;
  // compute number of missed samples:
  size_t missed = 0;
  if (head >= Tail && headcycle > TailCycle)
    missed = head - Tail + (headcycle-TailCycle-1)*Data->nbuffer();
  else if (head < Tail && headcycle > TailCycle+1)
    missed = Data->nbuffer() - Tail + head + (headcycle-TailCycle-2)*Data->nbuffer();
  if (missed > 0) {
    // update tail:
    Tail = head + Data->nchannels();
    TailCycle = headcycle - 1;
    if (Tail >= Data->nbuffer()) {
      Tail -= Data->nbuffer();
      TailCycle++;
    }
  }
  return missed;
}
