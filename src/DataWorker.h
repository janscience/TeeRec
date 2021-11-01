/*
  DataWorker - Producer/consumer working on a DataBuffer.
  Created by Jan Benda, November 2nd, 2021.
*/

#ifndef DataWorker_h
#define DataWorker_h


#include <Arduino.h>


class DataBuffer;


// Base class for all objects producing or consuming data on a cyclic buffer.
class DataWorker {
  
public:

  DataWorker();
  DataWorker(const DataWorker *producer);

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


protected:

  // Set current index to the one of the data producer.
  // If no producer is available yet, return false.
  bool synchronize();
  
  // Increment current index by indices. Wrap around the buffer if
  // necessary and increment cycle counter.
  // Return true if the index was wrapped around.
  bool increment(size_t indices);
  
  volatile size_t Index;      // index into the buffer.
  volatile size_t Cycle;      // count buffer cycles.

  const DataBuffer *Data;     // pointer to the data buffer held by the initial data producer.

  const DataWorker *Producer; // pointer to producer providing data for this consumer.

  static const size_t MaxConsumers = 10;
  mutable size_t NConsumers;
  mutable DataWorker *Consumers[MaxConsumers];  
};


#endif
