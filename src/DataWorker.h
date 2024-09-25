/*
  DataWorker - Producer/consumer working on a DataBuffer.
  Created by Jan Benda, November 2nd, 2021.
*/

#ifndef DataWorker_h
#define DataWorker_h


#include <Arduino.h>
#include <WaveHeader.h>


class DataBuffer;


// Base class for all objects producing or consuming data on a cyclic buffer.
class DataWorker {
  
public:

  DataWorker(int verbose=0);
  DataWorker(const DataWorker *producer, int verbose=0);

  // Set producer from which data should be further processed.
  void setProducer(const DataWorker *producer);

  // Add consumer that work on the data processed by this class.
  void addConsumer(DataWorker *consumer) const;

  // Reset data buffer and dependent consumers.
  virtual void reset();

  // Return current value of the index (interrupt protected).
  size_t index() const;

  // Return current value of the cycle counter (interrupt protected).
  size_t cycle() const;

  // Number of samples available for consumption of this class.
  size_t available() const;

  // Number of samples that have been missed to be consumed.
  // Sets the tail forward to the first still available sample.
  size_t overrun();

  // Set verbosity level. 0: no messages. The higher, the more messages you get.
  // Messages of this class are displayed for levels 3 and 4.
  void setVerbosity(int verbose);

  // Add metadata to the header of a wave file holding the data of the
  // buffer.
  // The default implementation calls this function of the producer.
  // When implementing this function, you should call this default
  // implementation first.
  virtual void setWaveHeader(WaveHeader &wave) const;


protected:

  // Set current index to the one of the data producer.
  // If no producer is available yet return false.
  bool synchronize();

  // Set current index and cycle counter to the ones of worker.
  void synchronize(const DataWorker &worker);

  // Decrement current index by indices samples. Wrap around the
  // buffer and decrement cycle counter if necessary.  Return true if
  // the index was wrapped back.  The index is decremented by the
  // buffer size at maximum and no further than to the beginning of
  // sampling.
  bool decrement(size_t indices);
  
  // Increment current index by indices samples. Wrap around the
  // buffer and increment cycle counter if necessary.  Return true if
  // the index was wrapped around.
  bool increment(size_t indices);
  
  volatile size_t Index;      // index into the buffer.
  volatile size_t Cycle;      // count buffer cycles.

  const DataBuffer *Data;     // pointer to the data buffer held by the initial data producer.

  const DataWorker *Producer; // pointer to producer providing data for this consumer.

  static const size_t MaxConsumers = 10;
  mutable size_t NConsumers;
  mutable DataWorker *Consumers[MaxConsumers];

  int Verbose;
  
};


#endif
