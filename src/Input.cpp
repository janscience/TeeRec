#include <Arduino.h>
#include <Input.h>


Input::Input(volatile sample_t *buffer, size_t nbuffer, size_t dmabuffer) :
  DataBuffer(buffer, nbuffer, dmabuffer),
  Running(false),
  StartTime(0) {
}


bool Input::check(uint8_t nchannels, Stream &stream) {
  if (NChannels < 1) {
    stream.println("ERROR: no channels specified.");
    Rate = 0;
    NChannels = 0;
    return false;
  }
  if (nchannels > 0 && NChannels != nchannels) {
    stream.printf("ERROR: number of configured channels %d does not match number of requested channels %d\n", NChannels, nchannels);
    Rate = 0;
    NChannels = 0;
    return false;
  }
  return true;
}


void Input::begin(Stream &stream) {
}


void Input::start() {
  Running = true;
  StartTime = millis();
}


void Input::stop() {
  Running = false;
}


