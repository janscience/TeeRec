#include <Arduino.h>
#include <DataBuffer.h>


DataBuffer::DataBuffer(volatile sample_t *buffer, size_t nbuffer,
		       size_t dmabuffer)
  : DataWorker() {
  NBuffer = nbuffer;
  Buffer = buffer;
  DataBits = 16;
  Bits = DataBits;
  Rate = 0;
  NChannels = 0;
  NConsumers = 0;
  memset((void *)Buffer, 0, sizeof(sample_t)*NBuffer);
  Data = this;
  Producer = 0;
  NDMABuffer = dmabuffer;
}


void DataBuffer::setRate(uint32_t rate) {
  Rate = rate;
}


void DataBuffer::setNChannels(uint8_t nchannels) {
  NChannels = nchannels;
}


void DataBuffer::setResolution(uint8_t bits) {
  Bits = bits;
}


void DataBuffer::setDataResolution(uint8_t bits) {
  DataBits = bits;
}


float DataBuffer::bufferTime() const {
  return float(NBuffer/NChannels)/Rate;
}


float DataBuffer::DMABufferTime() const {
  return float(NDMABuffer/NChannels)/Rate;
}


void DataBuffer::setDMABufferSize(size_t samples) {
  NDMABuffer = samples;
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


float DataBuffer::sampledTime() const {
  float sindex = cycle();
  sindex *= NBuffer;
  sindex += index();
  return sindex/NChannels/Rate;
}


size_t DataBuffer::currentSample(size_t decr) const {
  size_t idx = index();
  if (decr > 0) {
    idx += NBuffer - decr*NChannels;
    while (idx > NBuffer)
      idx -= NBuffer;
  }
  return idx;
}


size_t DataBuffer::decrementSample(size_t idx, size_t decr) const {
  idx += NBuffer - decr*NChannels;
  while (idx > NBuffer)
    idx -= NBuffer;
  return idx;
}


size_t DataBuffer::incrementSample(size_t idx, size_t incr) const {
  idx += incr*NChannels;
  while (idx > NBuffer)
    idx -= NBuffer;
  return idx;
}


void DataBuffer::getData(uint8_t channel, size_t start,
			 sample_t *buffer, size_t nframes) const {
  if (Rate == 0 || NChannels == 0) {
    memset(buffer, 0, sizeof(sample_t)*nframes);
    return;
  }
  if (nframes*NChannels > NBuffer) {
    Serial.println("ERROR: requested too many samples.");
    memset(buffer, 0, sizeof(sample_t)*nframes);
    return;
  }
  // copy:
  start += channel;
  for (size_t k=0; k<nframes; k++) {
    if (start >= NBuffer)
      start -= NBuffer;
    buffer[k] = Buffer[start];
    start += NChannels;
  }
}


void DataBuffer::getData(uint8_t channel, size_t start,
			 float *buffer, size_t nframes) const {
  if (Rate == 0 || NChannels == 0) {
    memset(buffer, 0, sizeof(float)*nframes);
    return;
  }
  if (nframes*NChannels > NBuffer) {
    Serial.println("ERROR: requested too many samples.");
    memset(buffer, 0, sizeof(float)*nframes);
    return;
  }
  // copy:
  start += channel;
  float scale = 1.0/(1 << (DataBits-1));
  for (size_t k=0; k<nframes; k++) {
    if (start >= NBuffer)
      start -= NBuffer;
    buffer[k] = scale*Buffer[start];
    start += NChannels;
  }
}


void DataBuffer::printData(size_t start, size_t nframes,
			   Stream &stream) const {
  if (Rate == 0 || NChannels == 0)
    return;
  start *= NChannels;
  for (size_t k=0; k<nframes; k++) {
    if (start >= NBuffer)
      start -= NBuffer;
    stream.print(Buffer[start++]);
    for (uint8_t j=1; j<NChannels; j++) {
      stream.print(';');
      stream.print(Buffer[start++]);
    }
    stream.println();
  }
}


void DataBuffer::printData(uint8_t channel, size_t start, size_t nframes,
			   Stream &stream) const {
  if (Rate == 0 || NChannels == 0)
    return;
  start *= NChannels;
  start += channel;
  for (size_t k=0; k<nframes; k++) {
    if (start >= NBuffer)
      start -= NBuffer;
    stream.print(Buffer[start]);
    stream.println();
    start += NChannels;
  }
}


void DataBuffer::checkData(int32_t min, int32_t max) const {
  for (unsigned int k=0; k<NBuffer; k++) {
    sample_t data = Buffer[k];
    if (data < min)
      Serial.printf("%d: %d < %d\n", k, data, min);
    else if (data > max)
      Serial.printf("%d: %d > %d\n", k, data, max);
  }
}

