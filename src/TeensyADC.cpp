#include <Arduino.h>
#include <ADC_util.h>
#include <TeensyBoard.h>
#include <TeensyADC.h>


#ifdef TEENSY4
  #warning "TeensyADC.cpp: setupChannels(), setupDMA(), and startTimer() need to be implemented for Teensy4"
#endif


volatile DMAMEM uint16_t TeensyADC::ADCBuffer[2][NMajors*MajorSize];

DMAMEM uint8_t TeensyADC::SC1AChannels[2][MaxChannels] __attribute__((aligned(TeensyADC::MaxChannels)));

TeensyADC *TeensyADC::ADCC = 0;

DMASetting TeensyADC::DMASettings[2][NMajors];


TeensyADC::TeensyADC(volatile sample_t *buffer, size_t nbuffer,
		     int8_t channel0, int8_t channel1) :
  DataBuffer(buffer, nbuffer) {
  for (uint8_t adc=0; adc<2; adc++) {
    NChans[adc] = 0;
    DMAIndex[adc] = 0;
    DMACounter[adc] = 0;
    DataHead[adc] = 0;
  }
  memset(Channels, 0, sizeof(Channels));
  memset(SC1AChannels, 0, sizeof(SC1AChannels));
  memset(DMASettings, 0, sizeof(DMASettings));

  DataShift = 0;
  DataOffs = 0;
  DataScaling = true;
  Averaging = 1;
  ConversionSpeed = ADC_CONVERSION_SPEED::HIGH_SPEED;
  SamplingSpeed = ADC_SAMPLING_SPEED::HIGH_SPEED;
  Reference = ADC_REFERENCE::REF_3V3;
  Rate = 0;
  NChannels = 0;
  ADCUse = 0;
  ADCC = this;
  if (channel0 >= 0)
    setChannel(0, channel0);
  if (channel1 >= 0)
    setChannel(1, channel1);
}


TeensyADC::TeensyADC(volatile sample_t *buffer, size_t nbuffer,
		     const int8_t *channels0, const int8_t *channels1) :
  TeensyADC(buffer, nbuffer) {
  setChannels(0, channels0);
  setChannels(1, channels1);
}


void TeensyADC::setChannel(uint8_t adc, int8_t channel) {
  if (channel >= 0) {
    Channels[adc][0] = channel;
    NChans[adc] = 1;
    ADCUse |= (1 << adc);
  }
  else {
    NChans[adc] = 0;
    ADCUse &= ~(1 << adc);
  }
  NChannels = 0;
  for (uint8_t adc=0; adc<2; adc++)
    NChannels += NChans[adc];
}


void TeensyADC::addChannel(uint8_t adc, int8_t channel) {
  if (channel < 0)
    return;
  Channels[adc][NChans[adc]++] = channel;
  NChannels = 0;
  for (uint8_t adc=0; adc<2; adc++)
    NChannels += NChans[adc];
  ADCUse |= (1 << adc);
}


void TeensyADC::setChannels(uint8_t adc, const int8_t *channels) {
  NChans[adc] = 0;
  if (channels == 0)
    return;
  for (uint8_t k=0; k<MaxChannels && channels[k]>=0; k++)
    Channels[adc][NChans[adc]++] = channels[k];
  NChannels = 0;
  for (uint8_t adc=0; adc<2; adc++)
    NChannels += NChans[adc];
  if (NChans[adc] > 0)
    ADCUse |= (1 << adc);
}


void TeensyADC::clearChannels(uint8_t adc) {
  NChans[adc] = 0;
  NChannels = 0;
  for (uint8_t adc=0; adc<2; adc++)
    NChannels += NChans[adc];
  ADCUse &= ~(1 << adc);
}


void TeensyADC::clearChannels() {
  for (uint8_t adc=0; adc<2; adc++)
    NChans[adc] = 0;
  NChannels = 0;
  ADCUse = 0;
}


uint8_t TeensyADC::nchannels(uint8_t adc) const {
  if ( (ADCUse & (adc+1)) == adc+1 )
    return NChans[adc];
  return 0;
}


uint8_t TeensyADC::nchannels() const {
  return NChannels;
}


void TeensyADC::channelStr(int8_t pin, char *chan) const {
  analogPin(pin, chan);
}


void TeensyADC::channels(uint8_t adc, char *chans) const
{
  bool first = true;
  for (uint8_t k=0; k<NChans[adc]; k++) {
    if ( ! first )
      *chans++ = ',';
    chans += analogPin(Channels[adc][k], chans);
    first = false;
  }
  *chans = '\0';
}


void TeensyADC::channels(char *chans) const
{
  bool first = true;
  int nchan = NChans[0]>=NChans[1]?NChans[0]:NChans[1];
  for (uint8_t k=0; k<nchan; k++) {
    for (uint8_t adc=0; adc<2; adc++) {
      if (k < NChans[adc]) {
        if ( ! first )
	  *chans++ = ',';
        chans += analogPin(Channels[adc][k], chans);
        first = false;
      }
    }
  }
  *chans = '\0';
}


void TeensyADC::setRate(uint32_t rate) {
  DataBuffer::setRate(rate);
}


void TeensyADC::setScaling(bool scale) {
  DataScaling = scale;
}


void TeensyADC::unsetScaling() {
  DataScaling = false;
}


uint8_t TeensyADC::resolution() const {
  return ADCUse>0?Bits:0;
}


void TeensyADC::setResolution(uint8_t bits) {
  ADConv.adc[0]->setResolution(bits);
  DataBuffer::setResolution(ADConv.adc[0]->getResolution());
  setDataResolution();
}


void TeensyADC::setDataResolution() {
  if (DataScaling) {
    DataBits = 16;
    DataShift = DataBits - Bits;
    DataOffs = 0x8000;
  }
  else {
    DataBits = Bits;
    DataShift = 0;
    DataOffs = 0xFFFF << (DataBits - 1);
  }
}


void TeensyADC::setAveraging(uint8_t num) {
  Averaging = num;
}


uint8_t TeensyADC::averaging(void) const {
  return Averaging;
}


void TeensyADC::setConversionSpeed(ADC_CONVERSION_SPEED speed) {
  ConversionSpeed = speed;
}


const char *TeensyADC::conversionSpeedStr(ADC_CONVERSION_SPEED speed) {
  return getConversionEnumStr(speed);
}


const char *TeensyADC::conversionSpeedStr() const {
  return conversionSpeedStr(ConversionSpeed);
}


const char *TeensyADC::conversionSpeedShortStr(ADC_CONVERSION_SPEED speed) {
  switch (speed) {
#if defined(ADC_TEENSY_4)
#else
    case ADC_CONVERSION_SPEED::VERY_LOW_SPEED:
      return (const char *)"verylow";
#endif
    case ADC_CONVERSION_SPEED::LOW_SPEED:
      return (const char *)"low";
    case ADC_CONVERSION_SPEED::MED_SPEED:
      return (const char *)"med";
    case ADC_CONVERSION_SPEED::HIGH_SPEED:
      return (const char *)"high";
#if defined(ADC_TEENSY_4)
#else
    case ADC_CONVERSION_SPEED::VERY_HIGH_SPEED:
      return (const char *)"veryhigh";
#endif
#if defined(ADC_TEENSY_4)
    case ADC_CONVERSION_SPEED::ADACK_10:
      return (const char *)"adack10";
    case ADC_CONVERSION_SPEED::ADACK_20:
      return (const char *)"adack20";
#else
    case ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS:
      return (const char *)"high16";
    case ADC_CONVERSION_SPEED::ADACK_2_4:
      return (const char *)"adack24";
    case ADC_CONVERSION_SPEED::ADACK_4_0:
      return (const char *)"adack40";
    case ADC_CONVERSION_SPEED::ADACK_5_2:
      return (const char *)"adack52";
    case ADC_CONVERSION_SPEED::ADACK_6_2:
      return (const char *)"adack62";
#endif
  }
  return (const char *)"none";
}


const char *TeensyADC::conversionSpeedShortStr() const {
  return conversionSpeedShortStr(ConversionSpeed);
}


ADC_CONVERSION_SPEED TeensyADC::conversionSpeedEnum(const char *conversion) {
  char str[strlen(conversion)+1];
  for (size_t k=0; k<strlen(conversion)+1; k++)
    str[k] = tolower(conversion[k]);
  if (strcmp(str, "low") == 0)
    return ADC_CONVERSION_SPEED::LOW_SPEED;
  else if (strcmp(str, "med") == 0)
    return ADC_CONVERSION_SPEED::MED_SPEED;
  else if (strcmp(str, "high") == 0)
    return ADC_CONVERSION_SPEED::HIGH_SPEED;
#if defined(ADC_TEENSY_4)
#else
  else if (strcmp(str, "verylow") == 0)
    return ADC_CONVERSION_SPEED::VERY_LOW_SPEED;
  else if (strcmp(str, "veryhigh") == 0)
    return ADC_CONVERSION_SPEED::VERY_HIGH_SPEED;
#endif
#if defined(ADC_TEENSY_4)
  else if (strcmp(str, "adack10") == 0)
    return ADC_CONVERSION_SPEED::ADACK_10;
  else if (strcmp(str, "adack20") == 0)
    return ADC_CONVERSION_SPEED::ADACK_20;
#else
  else if (strcmp(str, "high16") == 0)
    return ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS;
  else if (strcmp(str, "adack24") == 0)
    return ADC_CONVERSION_SPEED::ADACK_2_4;
  else if (strcmp(str, "adack40") == 0)
    return ADC_CONVERSION_SPEED::ADACK_4_0;
  else if (strcmp(str, "adack52") == 0)
    return ADC_CONVERSION_SPEED::ADACK_5_2;
  else if (strcmp(str, "adack62") == 0)
    return ADC_CONVERSION_SPEED::ADACK_6_2;
#endif
  else
    return ADC_CONVERSION_SPEED::HIGH_SPEED;
}


void TeensyADC::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed = speed;
}


const char *TeensyADC::samplingSpeedStr(ADC_SAMPLING_SPEED speed) {
  return getSamplingEnumStr(speed);
}


const char *TeensyADC::samplingSpeedStr() const {
  return samplingSpeedStr(SamplingSpeed);
}


const char *TeensyADC::samplingSpeedShortStr(ADC_SAMPLING_SPEED speed) {
  switch (speed) {
  case ADC_SAMPLING_SPEED::VERY_LOW_SPEED:
    return (const char *)"verylow";
  case ADC_SAMPLING_SPEED::LOW_SPEED:
    return (const char *)"low";
  case ADC_SAMPLING_SPEED::MED_SPEED:
    return (const char *)"med";
  case ADC_SAMPLING_SPEED::HIGH_SPEED:
    return (const char *)"high";
  case ADC_SAMPLING_SPEED::VERY_HIGH_SPEED:
    return (const char *)"veryhigh";
#if defined(ADC_TEENSY_4) // Teensy 4
  case ADC_SAMPLING_SPEED::LOW_MED_SPEED:
    return (const char *)"lowmed";
  case ADC_SAMPLING_SPEED::MED_HIGH_SPEED:
    return (const char *)"medhigh";
  case ADC_SAMPLING_SPEED::HIGH_VERY_HIGH_SPEED:
    return (const char *)"highveryhigh";
#endif
  }
  return (const char *)"NONE";
}


const char *TeensyADC::samplingSpeedShortStr() const {
  return samplingSpeedShortStr(SamplingSpeed);
}


ADC_SAMPLING_SPEED TeensyADC::samplingSpeedEnum(const char *sampling) {
  char str[strlen(sampling)+1];
  for (size_t k=0; k<strlen(sampling)+1; k++)
    str[k] = tolower(sampling[k]);
  if (strcmp(str, "verylow") == 0)
    return ADC_SAMPLING_SPEED::VERY_LOW_SPEED;
  else if (strcmp(str, "low") == 0)
    return ADC_SAMPLING_SPEED::LOW_SPEED;
  else if (strcmp(str, "med") == 0)
    return ADC_SAMPLING_SPEED::MED_SPEED;
  else if (strcmp(str, "high") == 0)
    return ADC_SAMPLING_SPEED::HIGH_SPEED;
  else if (strcmp(str, "veryhigh") == 0)
    return ADC_SAMPLING_SPEED::VERY_HIGH_SPEED;
#if defined(ADC_TEENSY_4) // Teensy 4
  else if (strcmp(str, "lowmed") == 0)
    return ADC_SAMPLING_SPEED::LOW_MED_SPEED;
  else if (strcmp(str, "medhigh") == 0)
    return ADC_SAMPLING_SPEED::MED_HIGH_SPEED;
  else if (strcmp(str, "highveryhigh") == 0)
    return ADC_SAMPLING_SPEED::HIGH_VERY_HIGH_SPEED;
#endif
  else
    return ADC_SAMPLING_SPEED::HIGH_SPEED;
}


void TeensyADC::setReference(ADC_REFERENCE ref) {
  Reference = ref;
}


const char *TeensyADC::referenceStr(ADC_REFERENCE ref) {
  if (ref == ADC_REFERENCE::REF_3V3)
    return (const char *)"3.3V";
#ifndef TEENSY4
  else if (ref == ADC_REFERENCE::REF_1V2)
    return (const char *)"1.2V";
  else if (ref == ADC_REFERENCE::REF_EXT)
    return (const char *)"EXT";
#endif
  else
    return (const char *)"NONE";
}


const char *TeensyADC::referenceStr() const {
  return referenceStr(Reference);
}


ADC_REFERENCE TeensyADC::referenceEnum(const char *reference) {
  char str[strlen(reference)+1];
  for (size_t k=0; k<strlen(reference)+1; k++)
    str[k] = tolower(reference[k]);
  if (strcmp(str, "3.3V") == 0)
    return ADC_REFERENCE::REF_3V3;
#ifndef TEENSY4
  else if (strcmp(str, "1.2V") == 0)
    return ADC_REFERENCE::REF_1V2;
  else if (strcmp(str, "EXT") == 0)
    return ADC_REFERENCE::REF_EXT;
#endif
  else
    return ADC_REFERENCE::REF_3V3;
}


void TeensyADC::gainStr(char *gains, float pregain) {
  float range = 3300.0;
#ifndef TEENSY4
  if (ref == ADC_REFERENCE::REF_1V2)
    range = 1200.0;
#endif
  sprintf(gains, "%.2fmV", 0.5*range/pregain);
}


void TeensyADC::configure(const TeensyADCSettings &settings) {
  setRate(settings.rate());
  setResolution(settings.resolution());
  setAveraging(settings.averaging());
  setConversionSpeed(settings.conversionSpeed());
  setSamplingSpeed(settings.samplingSpeed());
  setReference(settings.reference());
}


void TeensyADC::setConfiguration(TeensyADCSettings &settings) {
  settings.setRate(rate());
  settings.setResolution(resolution());
  settings.setAveraging(averaging());
  settings.setConversionSpeed(conversionSpeed());
  settings.setSamplingSpeed(samplingSpeed());
  settings.setReference(reference());
}


size_t TeensyADC::counter(uint8_t adc) const {
  return DMACounter[adc];
}


void TeensyADC::pinAssignment() {
  Serial.println("pin ADC0 ADC1");
  for (int k=0; k<NAPins; k++) {
    Serial.printf("A%-2d    %d    %d\n", k, ADConv.adc[0]->checkPin(APins[k]),
		  ADConv.adc[1]->checkPin(APins[k]));
  }
}


bool TeensyADC::check() {
  if ( Rate < 1 ) {
    Serial.println("ERROR: no sampling rate specfied.");
    ADCUse = 0;
    return false;
  }
  if ( NBuffer < NMajors*MajorSize ) {
    Serial.printf("ERROR: no buffer allocated or buffer too small. NBuffer=%d\n", NBuffer);
    ADCUse = 0;
    return false;
  }
  if (bufferTime() < 0.1)
    Serial.printf("WARNING: buffer time %.0fms should be larger than 100ms!\n",
		  1000.0*bufferTime());
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      if (NChans[adc] == 0) {
	Serial.println("ERROR: no channels supplied.");
	ADCUse = 0;
	return false;
      }
      if ((NChans[adc] & (NChans[adc]-1)) > 0) {
	Serial.printf("ERROR: number of channels (%d) on ADC%d must be a power of two.\n",
		      NChans[adc], adc);
	ADCUse = 0;
	return false;
      }
      for(uint8_t i=0; i<NChans[adc]; i++) {
	if ( ! ADConv.adc[adc]->checkPin(Channels[adc][i]) ) {
	  Serial.printf("ERROR: invalid channel at index %d at ADC%d\n", i, adc);
	  ADCUse = 0;
	  return false;
	}
      }
    }
  }
  if ( (ADCUse & 3) == 3 && NChans[0] != NChans[1] ) {
    Serial.printf("ERROR: number of channels on both ADCs must be the same. ADC0: %d, ADC1: %d\n", NChans[0], NChans[1]);
    ADCUse = 0;
    return false;
  }
  if ((Averaging & (Averaging-1)) > 0 || Averaging == 2 || Averaging > 32) {
    Serial.printf("ERROR: averaging must be one of 0, 1, 4, 8, 16, 32\n");
    return false;
  }
  return true;
}

  
void TeensyADC::report() {
  char chans0[50];
  char chans1[50];
  channels(0, chans0);
  channels(1, chans1);
  if (chans0[0] == '\0')
    strcpy(chans0, "-");
  if (chans1[0] == '\0')
    strcpy(chans1, "-");
  float bt = bufferTime();
  char bts[20];
  if (bt < 1.0)
    sprintf(bts, "%.0fms", 1000.0*bt);
  else
    sprintf(bts, "%.2fs", bt);
  Serial.println("ADC settings:");
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  averaging:  %d\n", Averaging);
  Serial.printf("  conversion: %s\n", conversionSpeedStr());
  Serial.printf("  sampling:   %s\n", samplingSpeedStr());
  Serial.printf("  reference:  %s\n", referenceStr());
  Serial.printf("  ADC0:       %s\n", chans0);
  Serial.printf("  ADC1:       %s\n", chans1);
  Serial.printf("  Buffer:     %s\n", bts);
  Serial.println();
}


void TeensyADC::setWaveHeader(WaveHeader &wave) const {
  DataWorker::setWaveHeader(wave);
  char cs[100];
  channels(cs);
  wave.setChannels(cs);
  wave.setAveraging(averaging());
  wave.setConversionSpeed(conversionSpeedShortStr());
  wave.setSamplingSpeed(samplingSpeedShortStr());
  wave.setReference(referenceStr());
}

  
void TeensyADC::start() {
  // setup acquisition:
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      DataHead[adc] = 0;
      setupChannels(adc);
      setupADC(adc);
      setupDMA(adc);
    }
  }
  // start timer:
  if ( (ADCUse & 3) == 3 ) {
    DataHead[1] = 1;
#if defined(ADC_USE_PDB)
    startPDB(Rate*NChans[0]);
#else
    // TODO: make this a single function?!?
    for (uint8_t adc=0; adc<2; adc++)
      ADConv.adc[adc]->startTimer(Rate*NChans[adc]);
#endif
    Rate = ADConv.adc[0]->getTimerFrequency()/NChans[0];
  }
  else {
    for (uint8_t adc=0; adc<2; adc++) {
      if ( (ADCUse & (adc+1)) == adc+1 ) {
	ADConv.adc[adc]->startTimer(Rate*NChans[adc]);
#if defined(ADC_USE_PDB)
	NVIC_DISABLE_IRQ(IRQ_PDB); // we do not need the PDB interrupt
#endif
	Rate = ADConv.adc[adc]->getTimerFrequency()/NChans[adc];
      }
    }
  }
  reset();   // resets the buffer and consumers
             // (they also might want to know about Rate)
}


void TeensyADC::stop() {
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (1 << adc)) > 0 ) {
      ADConv.adc[adc]->disableDMA();
      ADConv.adc[adc]->stopTimer();
      if ( NChans[adc] > 1 )
	DMASwitch[adc].disable();
      DMABuffer[adc].disable();
      DMABuffer[adc].detachInterrupt();
    }
  }
}


uint8_t TeensyADC::adcs() const {
  return (ADCUse+1)/2;
}


void TeensyADC::setupChannels(uint8_t adc) {
  // translate to SC1A code:
  for(uint8_t i=0; i<NChans[adc]; i++) {
#ifdef TEENSY3
    uint8_t sc1a_pin = 0;
    if ( adc == 0 )
      sc1a_pin = ADConv.channel2sc1aADC0[Channels[adc][i]];
    else if ( adc == 1 )
      sc1a_pin = ADConv.channel2sc1aADC1[Channels[adc][i]];
    SC1AChannels[adc][i] = (sc1a_pin & ADC_SC1A_CHANNELS) + ADC_SC1_AIEN;
#endif
  }
  if ( NChans[adc] > 1 ) {
    // reorder:
    uint8_t temp = SC1AChannels[adc][0];
    for(uint8_t i=1; i<NChans[adc]; i++)
      SC1AChannels[adc][i-1] = SC1AChannels[adc][i];
    SC1AChannels[adc][NChans[adc]-1] = temp;
  }
  // configure for input:
  for(uint8_t i=0; i<NChans[adc]; i++)
    pinMode(Channels[adc][i], INPUT);
}


void TeensyADC::setupADC(uint8_t adc) {
  ADConv.adc[adc]->setReference(Reference);
  ADConv.adc[adc]->setResolution(Bits);
  Bits = ADConv.adc[adc]->getResolution();
  setDataResolution();
  ADConv.adc[adc]->setAveraging(Averaging);
  ADConv.adc[adc]->setConversionSpeed(ConversionSpeed);  
  ADConv.adc[adc]->setSamplingSpeed(SamplingSpeed);
  ADConv.adc[adc]->disableDMA();
  ADConv.adc[adc]->stopTimer();
  ADConv.adc[adc]->startSingleRead(Channels[adc][0]);
  ADConv.adc[adc]->enableDMA();
}

    
void TeensyADC::setupDMA(uint8_t adc) {
  // From the K64 manual:
  // (https://www.mouser.com/datasheet/2/813/K64P144M120SF5RM-1074828.pdf, page 516)
  // CITER and BITER field:
  // bit 15: ELINK  set by triggerAtTransferOf() !
  // bit 12-9: LINKCH  in case of ELINK, dma channel number
  // If ELINK, then only 9 bit are available for citer/biter counter (512)
  // otherwise 15 bit are available (32768).

  DMAIndex[adc] = 0;
  DMACounter[adc] = 0;

  DMABuffer[adc].begin();
  DMABuffer[adc].disable();
  for (size_t mi=0; mi<NMajors; mi++) {
#ifdef TEENSY3
    DMASettings[adc][mi].source(adc==0?ADC0_RA:ADC1_RA);
#endif
    DMASettings[adc][mi].destinationBuffer(&ADCBuffer[adc][mi*MajorSize], sizeof(uint16_t)*MajorSize);
    DMASettings[adc][mi].transferSize(sizeof(uint16_t));
    DMASettings[adc][mi].replaceSettingsOnCompletion(DMASettings[adc][(mi+1)%NMajors]);
    DMASettings[adc][mi].interruptAtCompletion();
  }
  DMABuffer[adc] = DMASettings[adc][0];
#ifdef TEENSY3
  DMABuffer[adc].triggerAtHardwareEvent(adc==0?DMAMUX_SOURCE_ADC0:DMAMUX_SOURCE_ADC1);
#endif
  DMABuffer[adc].attachInterrupt(adc==0?DMAISR0:DMAISR1);
  DMABuffer[adc].enable();

  if ( NChans[adc] > 1 ) {
    DMASwitch[adc].begin();
    DMASwitch[adc].sourceCircular(SC1AChannels[adc], NChans[adc]);
#ifdef TEENSY3
    DMASwitch[adc].destination(adc==0?ADC0_SC1A:ADC1_SC1A); // this switches channels
    DMASwitch[adc].transferSize(1);
    DMASwitch[adc].triggerAtHardwareEvent(adc==0?DMAMUX_SOURCE_ADC0:DMAMUX_SOURCE_ADC1); 
#endif
    DMASwitch[adc].enable();
  }
}


void TeensyADC::isr(uint8_t adc) {
  // takes 31us! (=32kHz) for 256 samples
  size_t dmai = DMAIndex[adc]*MajorSize;
  DMAIndex[adc]++;
  if ( DMAIndex[adc] >= NMajors)
    DMAIndex[adc] = 0;
  // transform and copy DMA buffer:
  size_t step = 1;
  if (ADCUse == 3)
    step = 2;
  if (DataHead[adc] >= NBuffer)
    DataHead[adc] -= NBuffer;
  for (size_t k=0; k<MajorSize; k++) {
    uint16_t val = ADCBuffer[adc][dmai++];
    val <<= DataShift;  // make 16 bit
    val += DataOffs;      // convert to signed int
    Buffer[DataHead[adc]] = val;
    DataHead[adc] += step;
  }
  DMACounter[adc]++;
  if (ADCUse == 3) {
    if (DMACounter[0] == DMACounter[1]) {
      if (DataHead[0] < Index)
	Cycle++;
      Index = DataHead[0];
    }
  } else {
    if (DataHead[adc] < Index)
      Cycle++;
    Index = DataHead[adc];
  }
  DMABuffer[adc].clearInterrupt();
}


#if defined(ADC_USE_PDB)

void TeensyADC::startPDB(uint32_t freq) {
  if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) // setup PDB
    SIM_SCGC6 |= SIM_SCGC6_PDB;     // enable PDB clock

  if (freq > ADC_F_BUS)
    return; // too high
  if (freq < 1)
    return; // too low

  // mod will have to be a 16 bit value
  // we detect if it's higher than 0xFFFF and scale it back accordingly.
  uint32_t mod = ADC_F_BUS / freq;

  uint8_t prescaler = 0; // from 0 to 7: factor of 1, 2, 4, 8, 16, 32, 64 or 128
  uint8_t mult = 0;      // from 0 to 3, factor of 1, 10, 20 or 40

  // if mod is too high we need to use prescaler and mult to bring it down to a 16 bit number:
  const uint32_t min_level = 0xFFFF;
  if (mod > min_level) {
    if (mod < 2 * min_level)
      prescaler = 1;
    else if (mod < 4 * min_level)
      prescaler = 2;
    else if (mod < 8 * min_level)
      prescaler = 3;
    else if (mod < 10 * min_level)
      mult = 1;
    else if (mod < 16 * min_level)
      prescaler = 4;
    else if (mod < 20 * min_level)
      mult = 2;
    else if (mod < 32 * min_level)
      prescaler = 5;
    else if (mod < 40 * min_level)
      mult = 3;
    else if (mod < 64 * min_level)
      prescaler = 6;
    else if (mod < 128 * min_level)
      prescaler = 7;
    else if (mod < 160 * min_level) { // 16*10
      prescaler = 4;
      mult = 1;
    }
    else if (mod < 320 * min_level) { // 16*20
      prescaler = 4;
      mult = 2;
    }
    else if (mod < 640 * min_level) { // 16*40
      prescaler = 4;
      mult = 3;
    }
    else if (mod < 1280 * min_level) { // 32*40
      prescaler = 5;
      mult = 3;
    }
    else if (mod < 2560 * min_level) { // 64*40
      prescaler = 6;
      mult = 3;
    }
    else if (mod < 5120 * min_level) { // 128*40
      prescaler = 7;
      mult = 3;
    }
    else  // frequency too low
      return;
    mod >>= prescaler;
    if (mult > 0) {
      mod /= 10;
      mod >>= (mult - 1);
    }
  }

  ADConv.adc0->setHardwareTrigger();
  ADConv.adc1->setHardwareTrigger();

  //                                  software trigger    enable PDB     PDB interrupt  continuous mode load immediately
  constexpr uint32_t ADC_PDB_CONFIG = PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE | PDB_SC_CONT | PDB_SC_LDMOD(0);

  constexpr uint32_t PDB_CHnC1_TOS_1 = 0x0100;
  constexpr uint32_t PDB_CHnC1_EN_1 = 0x01;

  PDB0_IDLY = 1; // the pdb interrupt happens when IDLY is equal to CNT+1

  PDB0_MOD = (uint16_t)(mod - 1);

  PDB0_SC = ADC_PDB_CONFIG | PDB_SC_PRESCALER(prescaler) | PDB_SC_MULT(mult) | PDB_SC_LDOK; // load all new values

  PDB0_SC = ADC_PDB_CONFIG | PDB_SC_PRESCALER(prescaler) | PDB_SC_MULT(mult) | PDB_SC_SWTRIG; // start the counter!

  PDB0_CH0C1 = PDB_CHnC1_TOS_1 | PDB_CHnC1_EN_1; // enable pretrigger 0 (SC1A)
  PDB0_CH1C1 = PDB_CHnC1_TOS_1 | PDB_CHnC1_EN_1; // enable pretrigger 0 (SC1A)

  //NVIC_ENABLE_IRQ(IRQ_PDB);
}

#endif


void DMAISR0() {
  TeensyADC::ADCC->isr(0);
}


void DMAISR1() {
  TeensyADC::ADCC->isr(1);
}

