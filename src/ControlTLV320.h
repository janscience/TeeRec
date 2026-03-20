/*
  ControlTLV320 - Control a TI TLV320 chip.
  Created by Jan Benda, March 20th, 2026.
*/

#ifndef ControlTLV320_h
#define ControlTLV320_h


#include <Arduino.h>
#include <Wire.h>
#include <Device.h>
#include <InputTDM.h>


#define TLV320_I2C_ADDR1     0x4C
#define TLV320_I2C_ADDR2     0x4D
#define TLV320_I2C_ADDR3     0x4E
#define TLV320_I2C_ADDR4     0x4F


class ControlTLV320 : public Device {
  
public:

  static const size_t MaxSamplingRates = 9;
  static const uint32_t SamplingRates[MaxSamplingRates];
  
  enum DATA_BITS : uint8_t {
    BIT16,
    BIT20,
    BIT24,
    BIT32
  };

  enum SOURCE : uint8_t {
    DIFFERENTIAL_INPUT,
    SINGLE_ENDED_INPUT,
    DIGITAL_INPUT
  };

  enum IMPEDANCE : uint8_t {
    IMP_025,   // 2.5kOhm
    IMP_100,   // 10kOhm
    IMP_200    // 20kOhm
  };

  enum COUPLING : uint8_t {
    AC_CPL,
    DC_CPL
  };

  enum HIGHPASS : uint8_t {
    CUSTOM,
    LOW_HP,
    MED_HP,
    HIGH_HP
  };

  enum LOWPASS : uint8_t {
    LINEAR,
    LOW_LATENCY,
    ULTRALOW_LATENCY
  };
  

  /* Do not initialize TLV320 yet. */
  ControlTLV320();

  /* Communicate with TLV chip using defaut I2C bus
     and address (one of TLV320_I2C_ADDR*). */
  ControlTLV320(uint8_t address, InputTDM::TDM_BUS bus=InputTDM::TDM1);

  /* Communicate with TLV chip using wire, address (0x4A or 0x4B),
     and TDM bus. */
  ControlTLV320(TwoWire &wire, uint8_t address=TLV320_I2C_ADDR1,
		InputTDM::TDM_BUS bus=InputTDM::TDM1);

  /* Initialize TLV320 with already provided address and I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin();
  
  /* Initialize TLV320 with address (0x4A or 0x4B) on default I2C bus.
     You need to initialize I2C by calling `Wire.begin()` before. */
  bool begin(uint8_t address);

  /* Initialize TLV320 with address (0x4A or 0x4B) on I2C bus.
     You need to initialize I2C by calling `wire.begin()` before. */
  bool begin(TwoWire &wire, uint8_t address=TLV320_I2C_ADDR1);
  
  /* Set sampling rate per channel in Hertz. */
  void setRate(InputTDM &tdm, uint32_t rate);

  /* Set resolution. Call before seting up channels. */
  void setResolution(DATA_BITS bits);

  /* Return the input channel as a string. */
  const char *channelStr(uint8_t channel);

  /* Return the input channels set for each output channel
     as a string in chans with maximum nchans characters.
     If provided, prepend prefix to each channel. */
  void channelsStr(char *chans, size_t nchans, const char *prefix=0);
  
  /* Setup input channel as output channel. */
  bool setupChannel(uint8_t channel, SOURCE source, IMPEDANCE impedance,
		    COUPLING coupling, bool dre=false,
		    int8_t slot=-1, uint8_t offs=0);
  
  /* Setup input channels as output channels. */
  bool setupChannels(uint8_t n_chans, SOURCE source, IMPEDANCE impedance,
		     COUPLING coupling, bool dre=false,
		     int8_t slot=-1, uint8_t offs=0);

  /* Setup I2S output.
     Get the recorded data with AudioInputI2S */
  bool setupI2S();
  
  /* Setup TDM output for the specified two input channels.
     If offset, shift the recorded data by two slots.
     Set resolution, number and identifiers of channels of tdm accordingly. */
  bool setupTDM(InputTDM &tdm, bool offs=false);

  /* The TDM bus on which this TLV320 chip transmits data. */
  InputTDM::TDM_BUS TDMBus() const { return Bus; };

  /* Convert a gain factor to gain level in decibel. */
  float gainToDecibel(float gain);

  /* Convert a gain level in decibel to a gain factor. */
  float gainFromDecibel(float level);
  
  /* Return the gain set for channel in dB. */
  float gainDecibel(uint8_t channel);

  /* Set gain of channel to level dB,
     between 0 and 42dB in steps of 1dB.
     Returns the set gain level in dB or NAN on failure. */
  float setGainDecibel(uint8_t channel, float level);
  
  /* Return the gain set for output channel adc as a factor. */
  float gain(uint8_t channel);

  /* Set gain of a channel as a factor, between 1 and 100.
     Returns the set gain or NAN on failure. */
  float setGain(uint8_t channel, float gain);
  
  /* Return the gain set for all channels in dB. */
  float gainDecibel();

  /* Set gain of all channels to gain in dB,
     between 0 and 42dB in steps of 1dB.
     Pass the actually set gain on to tdm and return it. */
  float setGainDecibel(InputTDM &tdm, float gain);
  
  /* Return the gain set for all channels as a factor. */
  float gain();

  /* Set gain of all channels to gain as a factor,
     between 1 and 100.
     Pass the actually set gain on to tdm and return it. */
  float setGain(InputTDM &tdm, float gain);

  /* If smooth then smoothly ramp to new gains. */
  bool setSmoothGainChange(bool smooth=true);
  
  /*! Setup digital low- and highpass filter.
      Highpass cutoff frequencies:
      LOW_HP: 0.00025*rate
      MED_HP: 0.002*rate
      HIGH_HP: 0.008*rate */
  bool setFilters(LOWPASS lowpass=LINEAR, HIGHPASS highpass=LOW_HP);

  /*! Enter power down mode. */
  bool powerdown();

  /*! Switch back into active mode. */
  bool powerup();

  /* Print state (all status registers) to Serial. */
  void printState();

  /* Print values of all DSP coefficients to Serial */
  void printDSPCoefficients();

  
protected:
    
  unsigned int read(uint16_t address);
  bool write(uint16_t address, uint8_t val);
  uint8_t goToPage(uint8_t page);

  bool setActive();

  float readCoefficient(uint8_t address);

  void setTDMChannelStr(InputTDM &tdm);
  
  TwoWire *I2CBus;
  uint8_t I2CAddress;
  uint8_t CurrentPage;
  uint32_t Rate;
  DATA_BITS Bits;
  uint8_t UseChannel[4];  // 0: unused, 1: single ended, 2: differential
  InputTDM::TDM_BUS Bus;
  static const int WriteDelay = 1;

  static const uint8_t BitBytes[4];
  static const char *LowpassStrings[3];
  static const char *OnOffStrings[2];
  char GainStr[8];
  
};


#endif

