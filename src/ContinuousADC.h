/*
  ContinuousADC - library for sampling from multiple pins into a single cyclic buffer.
  Created by Jan Benda, May 25th, 2021.
  Based on https://github.com/pedvide/ADC
  and on contributions from Stefan Mucha, Lydia Federman, and Sebastian Volkmer.

  Features
  --------
  
  - Continuous DMA-based data acquisition into a single multiplexed circular buffer.
  - Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
  - Highspeed timed acquisition up to 500kHz.
  - Conversion of data to signed 16bit for direct storage into WAV files.


  Setup
  -----
  ```
  #include <ContinuousADC.h>

  uint32_t samplingRate = 40000;  // samples per second and channel in Hertz
  uint8_t channels0 [] =  { A2, A3, A4, A5, -1 };      // input pins for ADC0, terminate with -1
  uint8_t channels1 [] =  { A16, A17, A18, A19, -1 };  // input pins for ADC1, terminate with -1

  ContinuousADC aidata;

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


  Teensy 3.5
  ----------

  Maximum sampling rates in kHz:

  channels0 channels1 16bit 12bit
  1         0           440   520
  2         0           185   210
  4         0            95   110
  8         0            45    50
  1         1           440   520
  2         2           160   220
  4         4            85    90
  8         8            40    45


  Output of `pinAssignment()`:

  pin ADC0 ADC1
  A0     1    0
  A1     1    0
  A2     1    1
  A3     1    1
  A4     1    0
  A5     1    0
  A6     1    0
  A7     1    0
  A8     1    0
  A9     1    0
  A10    1    1
  A11    0    1
  A12    0    1
  A13    0    1
  A14    1    0
  A15    1    0
  A16    0    1
  A17    0    1
  A18    0    1
  A19    0    1
  A20    0    1
  A21    1    0
  A22    0    1
*/

#ifndef ContinuousADC_h
#define ContinuousADC_h


#include <Arduino.h>
#include <ADC.h>
#include <DMAChannel.h>
#include <DataBuffer.h>
#include <Configurable.h>


class ContinuousADC : public DataBuffer, public Configurable {

 public:

  static ContinuousADC *ADCC;
  
  static const size_t MajorSize = 256;
  
  static const size_t MaxChannels = 8;

  // Initialize.
  ContinuousADC();
  
  // Configure for acquisition of a single channel.
  // channel is a pin specifier like A6, A19.
  void setChannel(uint8_t adc, uint8_t channel);

  // Configure for acquisition from several channels on a single ADC.
  // channels is an array with pin specifications like A6, A19,
  // terminated by -1.
  // The number of channels must be a power of two,
  // because the channels have to fit into the 256 samples DMA buffer.
  void setChannels(uint8_t adc, const int8_t *channels);

  // Return number of channels on specified ADC.
  uint8_t nchannels(uint8_t adc) const;

  // Return total number of channels multiplexed into the buffer.
  uint8_t nchannels() const;

  // Return in chans a string with the channels/pins sampled on ADC adc.
  void channels(uint8_t adc, char *chans) const;

  // Return in chans a string with the channels/pins sampled from both ADCs
  // in the order they are multiplexed into the buffer.
  void channels(char *chans) const;
  
  // Set the sampling rate per channel in Hertz.
  void setRate(uint32_t rate);
  
  // Set the resolution in bits per sample (valid values are 10, 12, 16 bit).
  void setResolution(uint8_t bits);

  // Return ADC resolution in bits per sample.
  uint8_t resolution() const;

  // Set the number of averages taken by each sample.
  // Can be 0, 4, 8, 16, or 32.
  void setAveraging(uint8_t num);

  // Return the number of averages taken by each sample.
  uint8_t averaging(void) const;

  // Set the conversion speed by chaning the ADC clock.
  // One of VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS,
  // HIGH_SPEED, VERY_HIGH_SPEED, ADACK_2_4, ADACK_4_0, ADACK_5_2 or ADACK_6_2.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   Sampling: Load an internal capacitor with the voltage you want to measure.
  //   The longer you let this capacitor be charged, the closest it will resemble the voltage.
  //   Conversion: Convert that voltage into a digital representation that is as close
  //   as possible to the selected resolution.
  void setConversionSpeed(ADC_CONVERSION_SPEED speed);

  // Return string describing the selected conversion speed.
  const char *conversionSpeedStr() const;

  // Return a short string describing the selected conversion speed.
  const char *conversionSpeedShortStr() const;

  // Translate conversion speed short string to conversion speed enum.
  ADC_CONVERSION_SPEED conversionSpeedEnum(const char *conversion) const;

  // Set the sampling speed.
  // Increase the sampling speed for low impedance sources, 
  // decrease it for higher impedance ones.
  // One of VERY_LOW_SPEED, LOW_SPEED, LOW_MED_SPEED, MED_SPEED, MED_HIGH_SPEED,
  // HIGH_SPEED, HIGH_VERY_HIGH_SPEED, VERY_HIGH_SPEED.
  // From https://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1:
  // The measurement of a voltage takes place in two steps:
  //   Sampling: Load an internal capacitor with the voltage you want to measure.
  //   The longer you let this capacitor be charged, the closest it will resemble the voltage.
  //   Conversion: Convert that voltage into a digital representation that is as close
  //   as possible to the selected resolution.
  void setSamplingSpeed(ADC_SAMPLING_SPEED speed);

  // Return string describing the selected sampling speed.
  const char *samplingSpeedStr() const;

  // Return a short string describing the selected sampling speed.
  const char *samplingSpeedShortStr() const;

  // Translate sampling speed short string to sampling speed enum.
  ADC_SAMPLING_SPEED samplingSpeedEnum(const char *sampling) const;

  // Set the voltage preference.
  // One of ADC_REFERENCE::REF_3V3 (default), ADC_REFERENCE::REF_1V2, or ADC_REFERENCE::REF_EXT
  // Teensy 4.x has only 3V3, on Teensy 3.x EXT equals 3V3.
  void setReference(ADC_REFERENCE ref);

  // Return string describing the selected voltage reference.
  const char *referenceStr() const;

  // Translate voltage reference string to reference enum.
  ADC_REFERENCE referenceEnum(const char *reference) const;

  // Print the assignment of AI pins to ADC0 and ADC1 to Serial.
  void pinAssignment();

  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on Serial.
  // If successfull, you may remove this check from your code.
  bool check();

  // Print current settings on Serial.
  void report();
 
  // Start the acquisition based on the channel, rate, and buffer settings.
  void start();

  // Stop acquisition.
  void stop();

  // Number of ADCs in use (0, 1, or 2).
  uint8_t adcs() const;

  // Configure ADC settings with the provided key-value pair.
  virtual void configure(const char *key, const char *val);

  // Interrupt service routine. For internal usage.
  void isr(uint8_t adc);


 protected:
  
  // ADC:
  uint8_t Channels[2][MaxChannels];
  static DMAMEM uint8_t SC1AChannels[2][MaxChannels] __attribute__((aligned(MaxChannels)));
  uint8_t NChans[2];
  uint8_t ADCUse;

  uint8_t Bits;
  uint8_t Averaging;
  ADC_CONVERSION_SPEED ConversionSpeed;
  ADC_SAMPLING_SPEED SamplingSpeed;
  ADC_REFERENCE Reference;

  ADC ADConv;
  
  // DMA:
  static const size_t NMajors = 2;
  DMAChannel DMABuffer[2]; // DMA channel for ADCBuffer
  DMAChannel DMASwitch[2]; // DMA channel for switching pins
  volatile size_t DMAIndex[2];     // currently active ADCBuffer segment
  volatile size_t DMACounter[2];   // total count of ADCBuffer segments
  volatile static DMAMEM uint16_t ADCBuffer[2][NMajors*MajorSize];  // circular destination buffer must be aligned on its size, which must be a power of two!
  static DMASetting DMASettings[2][NMajors];

  // Data (large buffer holding converted and multiplexed data from both ADCs):
  DataBuffer Data;
  size_t DataHead[2]; // current index for each ADC for writing. Only used in isr.
  uint8_t DataShift;  // number of bits ADC data need to be shifted to make them 16 bit.
  
  void setupChannels(uint8_t adc);
  void setupADC(uint8_t adc);
  void setupDMA(uint8_t adc);
#if defined(ADC_USE_PDB)
  void startPDB(uint32_t freq);   // start both ADCs from PDB at the same time
#else
  #error "Need to implement startTimer() for Teensy 4 for both ADCs"
#endif
};


void DMAISR0();
void DMAISR1();


#endif
