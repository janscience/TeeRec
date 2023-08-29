#include <Arduino.h>
#include <Input.h>


Input::Input(volatile sample_t *buffer, size_t nbuffer, size_t dmabuffer) :
  DataBuffer(buffer, nbuffer, dmabuffer),
  Running(false) {
}
