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
  uint8_t channels0[] =  { A2, A3, A4, A5, -1 };      // input pins for ADC0, terminate with -1
  uint8_t channels1[] =  { A16, A17, A18, A19, -1 };  // input pins for ADC1, terminate with -1

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
  Alternatively, you may write your own data consumer by subclassing
  DataConsumer declared in DataBuffer.h.


  Teensy 3.5
  ----------

  Maximum sampling rates in kHz with averaging of 1, high sampling and
  conversion speeds (measured with the maxrate example):

  channels0 channels1 10bit 12bit 16bit
  1         0           523   531   453 
  2         0           250   250   210 
  4         0           125   125   105 
  8         0            62    62    50 
  1         1           531   519   453 
  2         2           234   234   199 
  4         4           117   117   101 
  8         8            58    58    50 

  Note that in an application these maximum sampling rates can be
  substantially lower.

  Effect of averaging on sampling rate in kHz for 12bit,
  high conversion and sampling speed, and a single ADC:

  channels a=1 a=4 a=16
  1        531 148   35
  2        250  70   15
  4        125  35    7
  8         62  15    3

  Effect of conversion and sampling speeds on sampling rate in kHz for 12bit,
  averaging set to 1, and a single ADC:

  High conversion speed:

  channels high med low sampling speed
  1         531 457 386
  2         250 218 187
  4         125 109  93
  8          62  54  46

  Medium conversion speed:

  channels high med low sampling speed
  1         285 246 207
  2         136	121 101
  4          66	 62  50
  8          31	 27  23

  Low conversion speed:

  channels high med low sampling speed
  1         144 125 101
  2          70	 62  50
  4          35	 27  23
  8          15	 15  11

  Conversion speed, sampling speed and in particular averaging help to
  reduce noise. Noise also depends on the pin.
  This can be tested with the averaging example.

  For 40kHz and 12bit we get for the standard deviations of the raw
  integer readings of two channels:

  convers  sampling avrg   A2  A16
  veryhigh veryhigh    1  1.8  2.5
  veryhigh veryhigh    4  1.0  1.4
  veryhigh veryhigh    8  0.9  1.0
  veryhigh veryhigh   16  0.7  1.0
  veryhigh veryhigh   32  0.6  0.9
  veryhigh high        1  2.1  2.3
  veryhigh high        4  0.8  1.0
  veryhigh high        8  0.6  0.7 *
  veryhigh high       16  0.5  0.5 **
  veryhigh med         1  1.7  2.3
  veryhigh med         4  0.8  0.9
  veryhigh med         8  0.6  0.8
  veryhigh med        16  0.6  0.6 *
  veryhigh low         1  1.8  2.5
  veryhigh low         4  0.7  1.0
  veryhigh low         8  0.6  0.7 *
  veryhigh low        16  0.6  0.6 *
  veryhigh verylow     1  1.9  2.3
  veryhigh verylow     4  0.7  0.8
  veryhigh verylow     8  0.6  0.7
  veryhigh verylow    16  0.7  0.8
  high     veryhigh    1  1.7  2.4
  high     veryhigh    4  0.8  1.1
  high     veryhigh    8  0.7  1.0
  high     veryhigh   16  0.7  1.0
  high     high        1  1.7  2.3
  high     high        4  0.8  0.9
  high     high        8  0.7  0.7
  high     med         1  1.9  2.3
  high     med         4  0.8  0.9
  high     med         8  0.6  0.7
  high     low         1  1.7  2.2
  high     low         4  0.8  0.9
  high     low         8  0.6  0.8
  high     verylow     1  1.7  2.3
  high     verylow     4  0.9  1.0
  high     verylow     8  0.6  0.7
  med      veryhigh    1  1.8  2.4
  med      veryhigh    4  0.9  1.1
  med      veryhigh    8  2.6  2.5
  med      high        1  1.8  2.2
  med      high        4  1.0  1.3
  med      high        8  0.8  1.0
  med      med         1  3.4  2.5
  med      med         4  1.0  1.5
  med      low         1  1.7  2.3
  med      low         4  0.8  1.0
  med      verylow     1  2.8  3.5
  med      verylow     4  2.7  3.1
  low      veryhigh    1  3.2  3.8
  low      veryhigh    4  2.9  3.3
  low      high        1  1.8  2.6
  low      high        4  1.2  1.8
  low      med         1  1.7  2.5
  low      low         1  1.8  2.4
  low      verylow     1  1.6  2.4
  verylow  veryhigh    1  1.7  2.5
  verylow  veryhigh    4  1.1  1.9
  verylow  high        1  1.7  2.2
  verylow  high        4  1.0  1.3
  verylow  med         1  3.8  4.2
  verylow  low         1  2.6  3.1
  verylow  verylow     1  1.8  2.2

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
  A23    0    1
  A24    0    1
  A25    0    0
  A26    0    0
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
  // Use pinAssignment() to see which pins are available for each ADC.
  // Use check() to ensure a valid channel configuration.
  void setChannel(uint8_t adc, uint8_t channel);

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
  void channels(char *chans) const;

  // If set true, ADC data are scaled to 16bit and are converted to
  // signed integers. Call this *before* setResolution().
  void setScaling(bool scale=true);
  
  // Do not scale ADC data to 16bit, keep the resolution requested by
  // setResoution(). Nevertheless convert ADC data to signed
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
  //   Sampling: Load an internal capacitor with the voltage you want to measure.
  //   The longer you let this capacitor be charged, the closest it will resemble the voltage.
  //   Conversion: Convert that voltage into a digital representation that is as close
  //   as possible to the selected resolution.
  void setConversionSpeed(ADC_CONVERSION_SPEED speed);

  // Return string describing a conversion speed.
  const char *conversionSpeedStr(ADC_CONVERSION_SPEED speed) const;

  // Return string describing the selected conversion speed.
  const char *conversionSpeedStr() const;

  // Return a short string describing a conversion speed.
  const char *conversionSpeedShortStr(ADC_CONVERSION_SPEED speed) const;

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

  // Return string describing a sampling speed.
  const char *samplingSpeedStr(ADC_SAMPLING_SPEED speed) const;
  
  // Return string describing the selected sampling speed.
  const char *samplingSpeedStr() const;

  // Return a short string describing the sampling speed.
  const char *samplingSpeedShortStr(ADC_SAMPLING_SPEED speed) const;

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

  // Return DMA counter for specified adc.
  size_t counter(uint8_t adc) const;

  // Print the assignment of AI pins to ADC0 and ADC1 to Serial.
  void pinAssignment();

  // Check validity of buffers and channels.
  // Returns true if everything is ok.
  // Otherwise print warnings on Serial.
  // If successfull, you may remove this check from your code.
  bool check();

  // Print current settings on Serial.
  void report();
 
  // Add metadata to the header of a wave file holding the data of the
  // buffer.
  virtual void setWaveHeader(WaveHeader &wave) const;

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

  // Data buffer:
  DataBuffer Data;    // large buffer holding converted and multiplexed data from both ADCs
  size_t DataHead[2]; // current index for each ADC for writing. Only used in isr.
  uint8_t DataShift;  // number of bits ADC data need to be shifted to make them 16 bit.
  uint16_t DataOffs;  // offset to be added to ADC data to convert them to signed integers.
  bool DataScaling;   // scale ADC data to 16bit.
  
  void setupChannels(uint8_t adc);
  void setupADC(uint8_t adc);
  void setupDMA(uint8_t adc);
#if defined(ADC_USE_PDB)
  void startPDB(uint32_t freq);   // start both ADCs from PDB at the same time
#else
  #error "Need to implement startTimer() for Teensy 4 for both ADCs"
#endif


private:
  
  // Set used resolution of data buffer in bits per sample based on
  // requested resolution and scaling.
  void setDataResolution();

};


void DMAISR0();
void DMAISR1();


#endif
