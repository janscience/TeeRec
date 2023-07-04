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
  
  enum DATA_BITS : uint8_t {
    BIT32,
    BIT24,
    BIT20,
    BIT16
  };

  enum LOWPASS : uint8_t {
    FIR,   // classic FIR (default)
    IIR    // low-latency IIR
  };
  

  /* Do not initialize PCM186x yet. */
  ControlPCM186x();
  
  /* Initialize PCM186x with address (0x4A or 0x4B) on default I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin(uint8_t address=PCM186x_I2C_ADDR);

  /* Initialize PCM186x with address (0x4A or 0x4B) on I2C bus.
     You need to initialize I2C by calling `wire.begin()` before. */
  bool begin(TwoWire &wire, uint8_t address=PCM186x_I2C_ADDR);
  
  /* Set input channel for output adc. */
  bool setChannel(OUTPUT_CHANNELS adc, INPUT_CHANNELS channel,
		  bool inverted=false);

  /* Setup I2S output for the specified two input channels.
     Get the recorded data with AudioInputI2S */
  bool setupI2S(INPUT_CHANNELS channel1, INPUT_CHANNELS channel2);
  
  /* Setup I2S output for the specified four input channels.
     Channels 3 and 4 are available as DOUT2 via GPIO0.
     Get the recorded data with AudioInputI2SQuad. */
  bool setupI2S(INPUT_CHANNELS channel1, INPUT_CHANNELS channel2,
		INPUT_CHANNELS channel3, INPUT_CHANNELS channel4);
  
  /* Setup TDM output for the specified two input channels.
     Get the recorded data with AudioInputTDM on slots 0, 2.
     If offset, shift the recorded data such that they appear
     on slots 8, 10. */
  bool setupTDM(INPUT_CHANNELS channel1, INPUT_CHANNELS channel2,
		bool offs=false);
  
  /* Setup TDM output for the specified four input channels.
     Get the recorded data with AudioInputTDM on slots 0, 2, 4, 6.
     If offset, shift the recorded data such that they appear
     on slots 8, 10, 12, 14. */
  bool setupTDM(INPUT_CHANNELS channel1, INPUT_CHANNELS channel2,
		INPUT_CHANNELS channel3, INPUT_CHANNELS channel4,
		bool offs=false);

  /* Set gain of one or more adc channels to gain in dB,
     between -12 and 40 in steps of 0.5.
     If smooth then smoothly ramp to the new gains. */
  bool setGain(OUTPUT_CHANNELS adc, float gain, bool smooth=true);

  /*! Setup digital low- and highpass filter.
      Highpass is belwo 10Hz. */
  bool setFilters(LOWPASS lowpass=FIR, bool highpass=true);

  /*! Mute ADC outputs. */
  bool mute(OUTPUT_CHANNELS adcs);

  /*! Unmute ADC outputs. */
  bool unmute(OUTPUT_CHANNELS adcs);

  /*! Setup mic bias. */
  bool setMicBias(bool power=true, bool bypass=false);

  /* Print state (all status registers) to Serial. */
  void printState();

  /* Print values of all page 0x00 registers to Serial */
  void printRegisters();

  /* Print values of all DSP coefficients to Serial */
  void printDSPCoefficients();

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

  float readCoefficient(uint8_t address);
  

  TwoWire *I2CBus;
  uint8_t I2CAddress;
  uint8_t CurrentPage;
  bool PGALinked;
  
};


#endif

