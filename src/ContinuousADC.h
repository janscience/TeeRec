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
    aidata.setMaxFileTime(60);  // seconds
    aidata.check();
    aidata.start();
    aidata.report();
  }
  ```

  Now, acquisition is continuously running and the cyclic buffer is filled.
  Use aidata.getData() and aidata.writeData() functions to work with the acquired data.


  Teensy 3.5
  ----------

  Maximum sampling rates in kHz:

  channels0 channels1 16bit 12bit
  1         0           440   520
  2         0           185   210
  4         0            95   110
  8         0            45    50
  1         1           440   520
  2         2           160   180
  4         4            85    90
  8         8            40    45


  Output of pinAssignment():

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
#include <SdFat.h>


class ContinuousADC {

 public:

  static ContinuousADC *ADCC;

  // Initialize.
  ContinuousADC();
  
  // Configure for acquisition of a single channel.
  // channel is a pin specifier like A6, A19.
  void setChannel(uint8_t adc, uint8_t channel);

  // Configure for acquisition from several channels on a single ADC.
  // channels is an array with pin specifications like A6, A19.
  // nchannels must be a power of two,
  // because the channels have to fit into the 256 samples DMA buffer.
  void setChannels(uint8_t adc, const int8_t *channels);

  // Return number of channels on specified ADC.
  uint8_t nchannels(uint8_t adc) const;

  // Return total number of channels on both ADCs.
  uint8_t nchannels() const;

  // Return in chans a string with the channels/pins sampled on ADC adc.
  void channels(uint8_t adc, char *chans) const;

  // Return in chans a string with the channels/pins sampled from both ADCs
  // in the order they are multiplexed into the buffer.
  void channels(char *chans) const;
  
  // Set the sampling rate per channel in Hertz.
  void setRate(uint32_t rate);

  // Return sampling rate per channel in Hertz.
  uint32_t rate() const;
  
  // Set the resolution in bits per sample (valid values are 10, 12, 16 bit).
  void setResolution(uint8_t bits);

  // Return ADC resolution in bits per sample.
  uint8_t resolution() const;

  // Return resolution of data buffer in bits per sample (always 16 bits).
  uint8_t dataResolution() const;

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

  // Set the voltage preference.
  // One of ADC_REFERENCE::REF_3V3 (default), ADC_REFERENCE::REF_1V2, or ADC_REFERENCE::REF_EXT
  // Teensy 4.x has only 3V3, on Teensy 3.x EXT equals 3V3.
  void setReference(ADC_REFERENCE ref);

  // Return string describing the selected voltage reference.
  const char *referenceStr() const;

  // Time the cyclic buffer can hold in seconds.
  float bufferTime() const;

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

  // Number of DMA buffers filled so far.
  size_t counter(int adc) const;

  // Number of frames (samples of a single channel) corresponding to time.
  size_t frames(float time) const;


  // Return sample right after most current data value in data buffer.
  size_t currentSample(size_t decr=0);

  // Decrement sample index into data buffer by decr times number of channels.
  size_t decrementSample(size_t idx, size_t decr);

  // Increment sample index into data buffer by decr times number of channels.
  size_t incrementSample(size_t idx, size_t incr);

  // Get the nbuffer most recent data from a channel scaled to (-1, 1). <1ms
  void getData(uint8_t channel, size_t start, float *buffer, size_t nbuffer);
  
  // Write available data to file (if the file is open).
  // If maxFileSamples() is set (>0), then stop writing after that many samples. 
  // Returns number of written samples.
  size_t writeData(FsFile &file);

  // Start writing to a file from the current data position on.
  void startWrite();

  // Return current file size in samples.
  size_t fileSamples() const;

  // Return current file size in seconds.
  float fileTime() const;

  // Return current file size as a string displaying minutes and seconds.
  // str must hold at least 6 characters.
  void fileTimeStr(char *str) const;

  // Set maximum file size to a fixed number of samples modulo 256.
  void setMaxFileSamples(size_t samples);

  // Set maximum file size to approximately that many seconds.
  void setMaxFileTime(float secs);

  // Return actually used maximum file size in samples.
  size_t maxFileSamples() const;

  // Return true if maximum number of samples have been written
  // and a new file needs to be opened.
  bool endWrite();

  // Check whether data in the whole buffer are within the specified range (for debugging).
  void checkData(int32_t min, int32_t max);

  // Interrupt service routine. For internal usage.
  void isr(uint8_t adc);


 protected:

  // pins:
  static const int NPins = 24;
  static const int Pins[NPins];

  // ADC:
  static const int MaxChannels = 20;
  uint8_t Channels[2][MaxChannels];
  uint8_t SC1AChannels[2][MaxChannels];
  uint8_t NChannels[2];
  uint8_t ADCUse;

  uint8_t Bits;
  uint32_t Rate;
  uint8_t Averaging;
  ADC_CONVERSION_SPEED ConversionSpeed;
  ADC_SAMPLING_SPEED SamplingSpeed;
  ADC_REFERENCE Reference;

  ADC ADConv;
  
  // DMA:
  static const size_t MajorSize = 256;
  static const size_t NMajors = 2;
  DMAChannel DMABuffer[2];        // transfer data from ADCs to ADCBuffer
  DMAChannel DMASwitch[2];        // tell ADC from which pin to sample
  volatile size_t DMAIndex[2];    // currently active ADCBuffer segment
  volatile size_t DMACounter[2];  // total count of ADCBuffer segments
  volatile DMAMEM static uint16_t __attribute__((aligned(32))) ADCBuffer[2][NMajors*MajorSize];
  DMAChannel::TCD_t TCDs[2][NMajors] __attribute__ ((aligned (32))) ;

  // Data (large buffer holding converted and multiplexed data from both ADCs):
  const uint8_t DataBits = 16;
  typedef int16_t sample_t;
  static const size_t NBuffer = 64*1024;    // size of this buffer: 64kB
  volatile static sample_t Buffer[NBuffer]; // the one and only buffer
  volatile size_t BufferWrite[2];           // current index for each ADC for writing.
  size_t BufferRead;                        // index for reading the buffer and writing to file.
  volatile uint16_t DataShift;              // number of bits ADC data need to be shifted to make them 16 bit.

  size_t FileSamples;             // current number of samples stored in a file.
  size_t FileMaxSamples;          // maximum number of samples to be stored in a file.
  
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
