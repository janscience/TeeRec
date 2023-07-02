/*
  ControlPCM186x - Control the PCM186x chip.
  Created by Jan Benda, June 29th, 2023.
*/

#ifndef ControlPCM186x_h
#define ControlPCM186x_h


#include <Arduino.h>
#include <Wire.h>
#include <AudioControl.h>  // Teensy Audio library


#define PCM186x_I2C_ADDR     0x4A


//class ControlPCM186x : public AudioControl {
class ControlPCM186x {
  
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

  enum INPUT_CHANNELS : uint8_t {
    CH1L  = 0x01,
    CH1R  = 0x02,
    CH1   = 0x03,
    CH2L  = 0x04,
    CH2R  = 0x08,
    CH2   = 0x0C,
    CH3L  = 0x10,
    CH3R  = 0x20,
    CH3   = 0x30,
    CH4L  = 0x40,
    CH4R  = 0x80,
    CH4   = 0xC0,
    CHL   = 0x55,
    CHR   = 0xAA,
    CHLR  = 0xFF
  };

  enum OUTPUT_CHANNELS : uint8_t {
    ADC1L  = 0x01,
    ADC1R  = 0x02,
    ADC1   = 0x03,
    ADC2L  = 0x04,
    ADC2R  = 0x08,
    ADC2   = 0x0C,
    ADCL   = 0x05,
    ADCR   = 0x0A,
    ADCLR  = 0x0F
  };
  

  /* Do not initialize PCM186x yet. */
  ControlPCM186x();
  
  /* Initialize PCM186x with address (0x4A or 0x4B) on default I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin(uint8_t address=PCM186x_I2C_ADDR);

  /* Initialize PCM186x with address (0x4A or 0x4B) on I2C bus.
     You need to initialize I2C by calling `wire.begin()` before. */
  bool begin(TwoWire &wire, uint8_t address=PCM186x_I2C_ADDR);

  /* Set format for audio data transmission. */
  bool setDataFormat(DATA_FMT fmt=I2S, DATA_BITS bits=BIT16, bool offs=false);
  
  /* Set input channel for output adc. */
  bool setChannel(OUTPUT_CHANNELS adc, INPUT_CHANNELS channel,
		  bool inverted=false);

  /* Set gain of one or more adc channels to gain in dB,
     between -12 and 40 in steps of 0.5 */
  bool setGain(OUTPUT_CHANNELS adc, float gain);

  /* Print state (all status registers) to Serial. */
  void printState();

  /* Print values of all registers to Serial */
  void printRegisters();

  // AudioControl interface:
  /*
  virtual bool enable(void);
  virtual bool disable(void);
  virtual bool volume(float volume);      // volume 0.0 to 1.0
  virtual bool inputLevel(float volume);  // volume 0.0 to 1.0
  virtual bool inputSelect(int n);
  */
  
protected:
    
  unsigned int read(uint16_t address);
  bool write(uint16_t address, uint8_t val);
  uint8_t goToPage(uint8_t page);

  TwoWire *I2CBus;
  uint8_t I2CAddress;
  uint8_t CurrentPage;
  bool PGALinked;
  
};


#endif

