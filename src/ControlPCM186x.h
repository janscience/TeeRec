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

// input and output channels:
#define PCM186x_CH1L         0x01
#define PCM186x_CH1R         0x02
#define PCM186x_CH1          0x03
#define PCM186x_CH2L         0x04
#define PCM186x_CH2R         0x08
#define PCM186x_CH2          0x0C
#define PCM186x_CH3L         0x10
#define PCM186x_CH3R         0x20
#define PCM186x_CH3          0x30
#define PCM186x_CH4L         0x40
#define PCM186x_CH4R         0x80
#define PCM186x_CH4          0xC0
#define PCM186x_CHL          0x55
#define PCM186x_CHR          0xAA
#define PCM186x_CHLR         0xFF

class ControlPCM186x : public AudioControl {
  
public:

  enum DATA_FMT : uint8_t {
    I2S,
    LEFT,
    RIGHT,
    TDM
  };
  
  enum DATA_BITS : uint8_t {
    BIT32,
    BIT24,
    BIT20,
    BIT16
  };

  /* Do not initialize PCM186x yet. */
  ControlPCM186x();
  
  /* Initialize PCM186x with address (0x94 or 0x96) on default I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin(uint8_t address=PCM186x_I2C_ADDR);

  /* Initialize PCM186x with address (0x94 or 0x96) on I2C bus.
     You need to initialize I2C by calling `wire.begin()` before. */
  bool begin(TwoWire &wire, uint8_t address=PCM186x_I2C_ADDR);

  /* Set format for audio data transmission. */
  bool setDataFormat(DATA_FMT fmt=I2S, DATA_BITS bits=BIT16, bool offs=false);
  
  /* Set input channel for output adc. */
  bool setChannel(int adc, int channel, bool inverted=false);

  /* Set gain of one or more channels to gain in dB,
     between -12 and 40 in steps of 0.5 */
  bool setGain(int channel, float gain);

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
  uint8_t goToPage(uint8_t page);

  TwoWire *I2CBus;
  uint8_t I2CAddress;
  
};


#endif

