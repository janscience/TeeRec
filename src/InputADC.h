/*
  InputADC - library for sampling from multiple analog pins of the Teensy ADC into a single cyclic buffer.
  Created by Jan Benda, May 25th, 2021.
  Based on https://github.com/pedvide/ADC
  and on contributions from Stefan Mucha, Lydia Federman, and Sebastian Volkmer.

  Features
  --------
  
  - Use the two ADCs of the Teensy for reading analog inputs.
  - Continuous DMA-based data acquisition into a single multiplexed circular buffer.
  - Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
  - Highspeed timed acquisition up to 500kHz.
  - Conversion of data to signed 16bit for direct storage into WAV files.


  Setup
  -----
  ```
  #include <InputADC.h>

  uint32_t samplingRate = 40000;  // samples per second and channel in Hertz
  uint8_t channels0[] =  { A2, A3, A4, A5, -1 };      // input pins for ADC0, terminate with -1
  uint8_t channels1[] =  { A16, A17, A18, A19, -1 };  // input pins for ADC1, terminate with -1

  DATA_BUFFER(AIBuffer, NAIBuffer, 256*256)
  InputADC aidata(AIBuffer, NAIBuffer);

  void setup() {
    aidata.setChannels(0, channels0);
    aidata.setChannels(1, channels1);
    aidata.setRate(samplingRate);
    aidata.setResolution(12);   // 10bit, 12bit, 16bit 
    aidata.check();
    aidata.start();
    aidata.report();
  }
  ```

  Now, acquisition is continuously running and the cyclic buffer is
  filled.  Use, for example, `aidata.getData()` or
  `SDWriter::writeData()` to work with the acquired data.
  Alternatively, you may write your own data consumer by subclassing
  DataConsumer declared in DataBuffer.h.

  Properties and performance
  --------------------------

  See https://github.com/janscience/TeeRec/tree/main/docs/inputadc.md .

*/

#ifndef InputADC_h
#define InputADC_h


#include <Arduino.h>
#include <ADC.h>
#include <DMAChannel.h>
#include <Input.h>


class InputADC : public Input {

 public:

  static InputADC *ADCC;
  
  static const size_t MaxChannels = 8;

#if defined(ADC_TEENSY_4)
  static const size_t MaxConversions = 5;
#else
  static const size_t MaxConversions = 10;
#endif
  static const char *ConversionShortStrings[MaxConversions];
  static const ADC_CONVERSION_SPEED ConversionEnums[MaxConversions];

#if defined(ADC_TEENSY_4)
  static const size_t MaxSamplings = 8;
#else
  static const size_t MaxSamplings = 5;
#endif
  static const char *SamplingShortStrings[MaxSamplings];
  static const ADC_SAMPLING_SPEED SamplingEnums[MaxSamplings];

#if defined(ADC_TEENSY_4)
  static const size_t MaxReferences = 1;
#else
  static const size_t MaxReferences = 3;
#endif
  static const char *ReferenceStrings[MaxReferences];
  static const ADC_REFERENCE ReferenceEnums[MaxReferences];

  // Initialize and pass a buffer that has been created with the
  // DATA_BUFFER macro.
  // Use channel0 on ADC0 and channel1 on ADC (see setChannel()).
  InputADC(volatile sample_t *buffer, size_t nbuffer,
	   int8_t channel0=-1, int8_t channel1=-1);

  // Initialize and pass a buffer that has been created with the
  // DATA_BUFFER macro.
  // Use channels0 on ADC0 and channels1 on ADC (see setChannels()).
  InputADC(volatile sample_t *buffer, size_t nbuffer,
	   const int8_t *channels0, const int8_t *channels1);
  
  // Configure for acquisition of a single channel.
  // channel is a pin specifier like A6, A19.
  // Use pinAssignment() to see which pins are available for each ADC.
  // Disable the adc by providing a negative channel.
  // Use check() to ensure a valid channel configuration.
  void setChannel(uint8_t adc, int8_t channel);
  
  // Add a channel to the configuration for acquisition on ADC adc.
  // channel is a pin specifier like A6, A19.
  // Use pinAssignment() to see which pins are available for each ADC.
  // Use check() to ensure a valid channel configuration.
  void addChannel(uint8_t adc, int8_t channel);

  // Configure for acquisition from several channels on a single ADC.
  // channels is an array with pin specifications like A6, A19,
  // terminated by -1.
  // The number of channels must be a power of two, e.g. 1, 2, 4, or 8,
  // because the channels have to fit into the 256 samples DMA buffer.
  // If you intend to sample from both ADCs,
  // the number of channels on both ADCs must be the same.
  // Use pinAssignment() to see which pins are available for each ADC.
  // Use check() to ensure a valid channel configuration.
  void setChannels(uint8_t adc, const int8_t *channels);
  
  // Clear the channel configuration for a given ADC.
  void clearChannels(uint8_t adc);
  
  // Clear the channel configuration of both ADCs.
  void clearChannels();

  // Return number of channels on specified ADC.
  uint8_t nchannels(uint8_t adc) const;

  // Return total number of channels multiplexed into the buffer.
  uint8_t nchannels() const;

  // Return in chan the name of a pin.
  void channelStr(int8_t pin, char *chan) const;

  // Return in chans a string with the channels/pins sampled on ADC adc.
  void channels(uint8_t adc, char *chans) const;

  // Return in chans a string with the channels/pins sampled from both ADCs
  // in the order they are multiplexed into the buffer.
  virtual void channels(char *chans) const;

  // If set true, ADC data are scaled to 16bit and are converted to
  // signed integers. Call this *before* setResolution().
  void setScaling(bool scale=true);
  
  // Do not scale ADC data to 16bit, keep the resolution requested by
  // setResolution(). Nevertheless convert ADC data to signed
  // integers. Call this *before* setResolution().
  void unsetScaling();
  
  // Set the sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return ADC resolution in bits per sample.
  uint8_t resolution() const;
  
  // Set resolution of data acquisition in bits per sample
  // Valid values are 10, 12, or 16 bit.
  virtual void setResolution(uint8_t bits);

  // Set the number of averages taken by each sample.
  // Valid values are 0, 4, 8, 16, or 32.
  void setAveraging(uint8_t num);

  // Return the number of averages taken by each sample.
  uint8_t averaging(void) const;

  // Set the conversion speed by changing the ADC clock.
  // One of VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS,
  // HIGH_SPEED, VERY_HIGH_SPEED, ADACK_2_4, ADACK_4_0, ADACK_5_2 or ADACK_6_2.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   1. Sampling: Load an internal capacitor with the voltage you
  //      want to measure.  The longer you let this capacitor be
  //      charged, the closest it will resemble the voltage.
  //   2. Conversion: Convert that voltage into a digital
  //      representation that is as close as possible to the selected
  //      resolution.
  // The conversion speed will change the ADC clock, ADCK. And affects
  // all stages in the measurement.
  void setConversionSpeed(ADC_CONVERSION_SPEED speed);

  // The conversion speed.
  ADC_CONVERSION_SPEED conversionSpeed() const { return ConversionSpeed; };

  // Return string describing a conversion speed.
  static const char *conversionSpeedStr(ADC_CONVERSION_SPEED speed);

  // Return string describing the selected conversion speed.
  const char *conversionSpeedStr() const;

  // Return a short string describing a conversion speed.
  static const char *conversionSpeedShortStr(ADC_CONVERSION_SPEED speed);

  // Return a short string describing the selected conversion speed.
  const char *conversionSpeedShortStr() const;

  // Translate conversion speed short string to conversion speed enum.
  static ADC_CONVERSION_SPEED conversionSpeedEnum(const char *conversion);

  // Set the sampling speed.
  // Increase the sampling speed for low impedance sources, 
  // decrease it for higher impedance ones.
  // One of VERY_LOW_SPEED, LOW_SPEED, LOW_MED_SPEED, MED_SPEED, MED_HIGH_SPEED,
  // HIGH_SPEED, HIGH_VERY_HIGH_SPEED, VERY_HIGH_SPEED.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   1. Sampling: Load an internal capacitor with the voltage you
  //      want to measure.  The longer you let this capacitor be
  //      charged, the closest it will resemble the voltage.
  //   2. Conversion: Convert that voltage into a digital
  //      representation that is as close as possible to the selected
  //      resolution.
  // Usually you can increase the sampling speed if what you measure
  // has a low impedance. However, if the impedance is high you should
  // decrease the speed.
  void setSamplingSpeed(ADC_SAMPLING_SPEED speed);

  // The sampling speed.
  ADC_SAMPLING_SPEED samplingSpeed() const { return SamplingSpeed; };

  // Return string describing a sampling speed.
  static const char *samplingSpeedStr(ADC_SAMPLING_SPEED speed);
  
  // Return string describing the selected sampling speed.
  const char *samplingSpeedStr() const;

  // Return a short string describing the sampling speed.
  static const char *samplingSpeedShortStr(ADC_SAMPLING_SPEED speed);

  // Return a short string describing the selected sampling speed.
  const char *samplingSpeedShortStr() const;

  // Translate sampling speed short string to sampling speed enum.
  static ADC_SAMPLING_SPEED samplingSpeedEnum(const char *sampling);

  // Set the voltage preference.
  // One of ADC_REFERENCE::REF_3V3 (default), ADC_REFERENCE::REF_1V2, or ADC_REFERENCE::REF_EXT
  // Teensy 4.x has only 3V3, on Teensy 3.x EXT equals 3V3.
  void setReference(ADC_REFERENCE ref);

  // The voltage reference.
  ADC_REFERENCE reference() const { return Reference; };

  // Return string describing voltage reference.
  static const char *referenceStr(ADC_REFERENCE ref);

  // Return string describing the selected voltage reference.
  const char *referenceStr() const;

  // Translate voltage reference string to reference enum.
  static ADC_REFERENCE referenceEnum(const char *reference);

  /* Return the current gain as a string in gains.
     The gain of a possible preamplifier is specified by pregain. */
  void gainStr(char *gains, float pregain=1.0);

  // Return DMA counter for specified adc.
  size_t counter(uint8_t adc) const;

  // Print the assignment of AI pins to ADC0 and ADC1 to Serial.
  void pinAssignment();

  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on stream.
  // If successfull, you may remove this check from your code.
  virtual bool check(Stream &stream=Serial);

  // Print current settings on stream.
  virtual void report(Stream &stream=Serial);
 
  // Add metadata to the header of a wave file holding the data of the
  // buffer.
  virtual void setWaveHeader(WaveHeader &wave) const;

  // Start the acquisition based on the channel, rate, and buffer settings.
  virtual void start();

  // Stop acquisition.
  virtual void stop();

  // Number of ADCs in use (0, 1, or 2).
  uint8_t adcs() const;

  // Interrupt service routine. For internal usage.
  void isr(uint8_t adc);


 protected:
  
  // DMA:
  static const size_t NMajors = 2;
  static DMASetting DMASettings[2][NMajors];
  DMAChannel DMABuffer[2]; // DMA channel for ADCBuffer
  DMAChannel DMASwitch[2]; // DMA channel for switching pins
  volatile size_t DMAIndex[2];     // currently active ADCBuffer segment
  volatile size_t DMACounter[2];   // total count of ADCBuffer segments
  volatile static DMAMEM uint16_t ADCBuffer[2][NMajors*MajorSize];
  // circular destination buffer must be aligned on its size,
  // which must be a power of two! But we are not using a circular buffer.
  
  // ADC:
  uint8_t Channels[2][MaxChannels];
  static DMAMEM uint8_t SC1AChannels[2][MaxChannels] __attribute__((aligned(MaxChannels)));
  uint8_t NChans[2];
  uint8_t ADCUse;

  uint8_t Averaging;
  ADC_CONVERSION_SPEED ConversionSpeed;
  ADC_SAMPLING_SPEED SamplingSpeed;
  ADC_REFERENCE Reference;

  ADC ADConv;

  volatile size_t DataHead[2]; // current index for each ADC for writing. Only used in isr.
  uint8_t DataShift;  // number of bits ADC data need to be shifted to make them 16 bit.
  uint16_t DataOffs;  // offset to be added to ADC data to convert them to signed integers.
  bool DataScaling;   // scale ADC data to 16bit.
  
  void setupChannels(uint8_t adc);
  void setupADC(uint8_t adc);
  void setupDMA(uint8_t adc);
#if defined(ADC_USE_PDB)
  void startPDB(uint32_t freq);   // start both ADCs from PDB at the same time
#endif


private:
  
  // Set used resolution of data buffer in bits per sample based on
  // requested resolution and scaling.
  void setDataResolution();
  
};


void DMAISR0();
void DMAISR1();


#endif
