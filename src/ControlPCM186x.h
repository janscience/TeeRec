/*
  ControlPCM186x - Control the PCM186x chip.
  Created by Jan Benda, June 29th, 2023.
*/

#ifndef ControlPCM186x_h
#define ControlPCM186x_h


#include <Arduino.h>
#include <Wire.h>
#include <AudioControl.h>  // Teensy Audio library

#define PCM186x_I2C_ADDR     0x94


class ControlPCM186x : public AudioControl {
  
public:

  /* Do not initialize PCM186x yet. */
  ControlPCM186x();
  
  /* Initialize PCM186x with address (0x94 or 0x96) on default I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin(uint8_t address=PCM186x_I2C_ADDR);

  /* Initialize PCM186x with address (0x94 or 0x96) on I2C bus.
     You need to initialize I2C by calling `wire.begin()` before. */
  bool begin(TwoWire &wire, uint8_t address=PCM186x_I2C_ADDR);

  /* Print state (all status registers) to Serial. */
  void printState();

  // AudioControl interface:
  virtual bool enable(void);
  virtual bool disable(void);
  virtual bool volume(float volume);      // volume 0.0 to 1.0
  virtual bool inputLevel(float volume);  // volume 0.0 to 1.0
  virtual bool inputSelect(int n);

  
protected:
    
  unsigned int read(uint16_t address);
  bool write(uint16_t address, uint8_t val);
  bool goToPage(uint8_t page);

  TwoWire &I2CBus;
  uint8_t I2CAddress;
  
};


#endif

