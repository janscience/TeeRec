#include <Arduino.h>
#include <ADC_util.h>
#include <ContinuousADC.h>


const int ContinuousADC::Pins[NPins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
					A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23};

ContinuousADC *ContinuousADC::ADCC = 0;
volatile DMAMEM uint16_t __attribute__((aligned(32))) ContinuousADC::ADCBuffer[2][NMajors*MajorSize];
volatile ContinuousADC::sample_t ContinuousADC::Buffer[NBuffer];


ContinuousADC::ContinuousADC() {
  for (uint8_t adc=0; adc<2; adc++) {
    NChannels[adc] = 0;
    DMAIndex[adc] = 0;
    DMACounter[adc] = 0;
    BufferWrite[adc] = 0;
  }
  memset(Channels, 0, sizeof(Channels));
  memset(SC1AChannels, 0, sizeof(SC1AChannels));
  memset(TCDs, 0, sizeof(TCDs));

  Bits = DataBits;
  DataShift = 0;
  Averaging = 1;
  ConversionSpeed = ADC_CONVERSION_SPEED::HIGH_SPEED;
  SamplingSpeed = ADC_SAMPLING_SPEED::HIGH_SPEED;
  Rate = 0;
  ADCUse = 0;
  ADCC = this;

  BufferRead = 0;
  FileSamples = 0;
  FileMaxSamples = 0;
}


void ContinuousADC::setChannel(uint8_t adc, uint8_t channel) {
  Channels[adc][0] = channel;
  NChannels[adc] = 1;
  ADCUse |= (1 << adc);
}


void ContinuousADC::setChannels(uint8_t adc, const int8_t *channels) {
  NChannels[adc] = 0;
  for (uint8_t k=0; k<MaxChannels && channels[k]>0; k++)
    Channels[adc][NChannels[adc]++] = channels[k];
  if (NChannels[adc] > 0)
    ADCUse |= (1 << adc);
}


uint8_t ContinuousADC::nchannels(uint8_t adc) const {
  if ( (ADCUse & (adc+1)) == adc+1 )
    return NChannels[adc];
  return 0;
}


uint8_t ContinuousADC::nchannels() const {
  uint8_t nchans = 0;
  for (uint8_t adc=0; adc<2; adc++)
    nchans += nchannels(adc);
  return nchans;
}


void ContinuousADC::channels(uint8_t adc, char *chans) const
{
  bool first = true;
  for (uint8_t k=0; k<NChannels[adc]; k++) {
    int8_t ch = Channels[adc][k];
    for (int p=0; p<NPins; p++) {
      if (Pins[p] == ch) {
	if ( ! first )
	  *chans++ = ',';
	chans += sprintf(chans, "A%d", p);
	first = false;
	break;
      }
    }
  }
  *chans = '\0';
}


void ContinuousADC::channels(char *chans) const
{
  bool first = true;
  int nchan = NChannels[0]>=NChannels[1]?NChannels[0]:NChannels[1];
  for (uint8_t k=0; k<nchan; k++) {
    for (uint8_t adc=0; adc<2; adc++) {
      if (k<NChannels[adc]) {
	int8_t ch = Channels[adc][k];
	for (int p=0; p<NPins; p++) {
	  if (Pins[p] == ch) {
	    if ( ! first )
	      *chans++ = ',';
	    chans += sprintf(chans, "A%d", p);
	    first = false;
	    break;
	  }
	}
      }
    }
  }
  *chans = '\0';
}


void ContinuousADC::setRate(uint32_t rate) {
  Rate = rate;
}


uint32_t ContinuousADC::rate() const {
  return ADCUse>0?Rate:0;
}


void ContinuousADC::setResolution(uint8_t bits) {
  Bits = bits;
  DataShift = DataBits - Bits;
}


uint8_t ContinuousADC::resolution() const {
  return ADCUse>0?Bits:0;
}


uint8_t ContinuousADC::dataResolution() const {
  return DataBits;
}


void ContinuousADC::setAveraging(uint8_t num) {
  Averaging = num;
}


uint8_t ContinuousADC::averaging(void) const {
  return Averaging;
}


void ContinuousADC::setConversionSpeed(ADC_CONVERSION_SPEED speed) {
  ConversionSpeed = speed;
}


const char *ContinuousADC::conversionSpeedStr() const {
  return getConversionEnumStr(ConversionSpeed);
}


const char *ContinuousADC::conversionSpeedShortStr() const {
  switch (ConversionSpeed) {
#if defined(ADC_TEENSY_4) // Teensy 4
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
#if defined(ADC_TEENSY_4) // Teensy 4
#else
    case ADC_CONVERSION_SPEED::VERY_HIGH_SPEED:
      return (const char *)"veryhigh";
#endif
#if defined(ADC_TEENSY_4) // Teensy 4
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


void ContinuousADC::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
  SamplingSpeed = speed;
}


const char *ContinuousADC::samplingSpeedStr() const {
  return getSamplingEnumStr(SamplingSpeed);
}


const char *ContinuousADC::samplingSpeedShortStr() const {
  switch (SamplingSpeed) {
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


float ContinuousADC::bufferTime() const {
  return float(NBuffer/nchannels())/Rate;
}


void ContinuousADC::pinAssignment() {
  Serial.println("pin ADC0 ADC1");
  for (int k=0; k<NPins; k++) {
    Serial.printf("A%-2d    %d    %d\n", k, ADConv.adc[0]->checkPin(Pins[k]),
		  ADConv.adc[1]->checkPin(Pins[k]));
  }
}

  
bool ContinuousADC::check() {
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
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      if (NChannels[adc] == 0) {
	Serial.println("ERROR: no channels supplied.");
	ADCUse = 0;
	return false;
      }
      if ((NChannels[adc] & (NChannels[adc]-1)) > 0) {
	Serial.printf("ERROR: number of channels (%d) on ADC%d must be a power of two.\n",
		      NChannels[adc], adc);
	ADCUse = 0;
	return false;
      }
      for(uint8_t i=0; i<NChannels[adc]; i++) {
	if ( ! ADConv.adc[adc]->checkPin(Channels[adc][i]) ) {
	  Serial.printf("ERROR: invalid channel at index %d at ADC%d\n", i, adc);
	  ADCUse = 0;
	  return false;
	}
      }
    }
  }
  if ( (ADCUse & 3) == 3 && NChannels[0] != NChannels[1] ) {
    Serial.printf("ERROR: number of channels on both ADCs must be the same. ADC0: %d, ADC1: %d\n", NChannels[0], NChannels[1]);
    ADCUse = 0;
    return false;
  }
  if ((Averaging & (Averaging-1)) > 0 || Averaging == 2 || Averaging > 32) {
    Serial.printf("ERROR: averaging must be one of 0, 1, 4, 8, 16, 32\n");
    return false;
  }
  // report:
  char chans[100];
  channels(chans);
  Serial.println("ADC settings:");
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  averaging:  %d\n", Averaging);
  Serial.printf("  conversion: %s\n", conversionSpeedStr());
  Serial.printf("  sampling:   %s\n", samplingSpeedStr());
  Serial.printf("  ADC0:       %dchannel%s\n", NChannels[0], NChannels[0]>1?"s":"");
  Serial.printf("  ADC1:       %dchannel%s\n", NChannels[1], NChannels[1]>1?"s":"");
  Serial.printf("  Pins:       %s\n", chans);
  float bt = bufferTime();
  if (bt < 1.0)
    Serial.printf("  Buffer:     %.0fms\n", 1000.0*bt);
  else
    Serial.printf("  Buffer:     %.2fs\n", bt);
  Serial.println();
  return true;
}

  
void ContinuousADC::start() {
  BufferRead = 0;
  FileSamples = 0;
  memset((void *)Buffer, 0, sizeof(sample_t)*NBuffer);
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      BufferWrite[adc] = 0;
      setupChannels(adc);
      setupADC(adc);
      setupDMA(adc);
    }
  }
  if ( (ADCUse & 3) == 3 ) {
    BufferWrite[1] = 1;
#if defined(ADC_USE_PDB)
    startPDB(Rate*NChannels[0]);
#else
    // TODO: make this a single function?!?
    for (uint8_t adc=0; adc<2; adc++)
      ADConv.adc[adc]->startTimer(Rate*NChannels[adc]);
#endif
    Rate = ADConv.adc[0]->getTimerFrequency()/NChannels[0];
  }
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      if ( (ADCUse & 3) != 3 ) {
	ADConv.adc[adc]->startTimer(Rate*NChannels[adc]);
#if defined(ADC_USE_PDB)
	NVIC_DISABLE_IRQ(IRQ_PDB); // we don not need the PDB interrupt
#endif
	Rate = ADConv.adc[adc]->getTimerFrequency()/NChannels[adc];
      }
    }
  }
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 )
      DMABuffer[adc].enable();
  }
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 && NChannels[adc] > 1 )
      DMASwitch[adc].enable();
  }
}


void ContinuousADC::stop() {
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) > 0 ) {
      ADConv.adc[adc]->stopTimer();
      ADConv.adc[adc]->disableDMA();
      if ( NChannels[adc] > 1 )
	DMASwitch[adc].disable();
      DMABuffer[adc].disable();
      DMABuffer[adc].detachInterrupt();
    }
  }
}


size_t ContinuousADC::frames(float time) const {
  return floor(time*Rate);
}


uint8_t ContinuousADC::adcs() const {
  return (ADCUse+1)/2;
}


size_t ContinuousADC::counter(int adc) const {
  unsigned char sreg_backup = SREG;
  cli();
  size_t c = DMACounter[adc];
  SREG = sreg_backup;
  return c;
}


size_t ContinuousADC::currentSample(size_t decr) {
  unsigned char sreg_backup;
  size_t counter0;
  size_t counter1;
  size_t idx = 0;
  if ((ADCUse & 3) == 3) {
    while (true) {
      sreg_backup = SREG;
      cli();
      counter0 = DMACounter[0];
      counter1 = DMACounter[1];
      SREG = sreg_backup;
      if (counter0 == counter1)
	break;
    }
  }
  sreg_backup = SREG;
  cli();
  if ((ADCUse & 1) == 1)
    idx = BufferWrite[0];
  else if ((ADCUse & 2) == 2)
    idx = BufferWrite[1];
  SREG = sreg_backup;
  if (decr > 0) {
    idx += NBuffer - decr*(NChannels[0] + NChannels[1]);
    while (idx >= NBuffer)
      idx -= NBuffer;
  }
  return idx;
}


size_t ContinuousADC::decrementSample(size_t idx, size_t decr) {
  idx += NBuffer - decr*(NChannels[0] + NChannels[1]);
  while (idx >= NBuffer)
    idx -= NBuffer;
  return idx;
}


size_t ContinuousADC::incrementSample(size_t idx, size_t incr) {
  idx += incr*(NChannels[0] + NChannels[1]);
  while (idx >= NBuffer)
    idx -= NBuffer;
  return idx;
}


void ContinuousADC::getData(uint8_t channel, size_t start, float *buffer, size_t nbuffer) {
  if ( ADCUse == 0 ) {
    memset(buffer, 0, sizeof(sample_t)*nbuffer);
    return;
  }
  size_t step = NChannels[0] + NChannels[1];
  if (nbuffer*step > NBuffer) {
    Serial.println("ERROR: requested too many samples.");
    memset(buffer, 0, sizeof(sample_t)*nbuffer);
    return;
  }
  // copy:
  start += channel;
  float scale = 1.0/(1 << (DataBits-1));
  for (size_t k=0; k<nbuffer; k++ ) {
    if (start >= NBuffer)
      start -= NBuffer;
    buffer[k] = scale*Buffer[start];
    start += step;
  }
}


size_t ContinuousADC::writeData(FsFile &file) {
  size_t nbytes = 0;
  size_t samples0 = 0;
  size_t samples1 = 0;
  if ( ADCUse == 0 )
    return 0;
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return 0;
  if (! file.isOpen())
    return 0;
  size_t last = currentSample();
  size_t nwrite = 0;
  if (BufferRead >= last && ! (last == 0 && BufferRead ==0)) {
    nwrite = NBuffer - BufferRead;
    if (nwrite > 0) {
      if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
	nwrite = FileMaxSamples - FileSamples;
      nbytes = file.write((void *)&Buffer[BufferRead], sizeof(sample_t)*nwrite);
      samples0 = nbytes / sizeof(sample_t);
      BufferRead += samples0;
      if (BufferRead >= NBuffer)
	BufferRead -= NBuffer;
      FileSamples += samples0;
    }
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = last - BufferRead;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  if (nwrite > 0) {
    nbytes = file.write((void *)&Buffer[BufferRead], sizeof(sample_t)*nwrite);
    samples1 = nbytes / sizeof(sample_t);
    BufferRead += samples1;
    if (BufferRead >= NBuffer)
      BufferRead -= NBuffer;
    FileSamples += samples1;
  }
  return samples0 + samples1;
}


void ContinuousADC::startWrite() {
  BufferRead = currentSample();
  FileSamples = 0;
}


size_t ContinuousADC::fileSamples() const {
  return FileSamples;
}


float ContinuousADC::fileTime() const {
  return FileSamples/float(NChannels[0]+NChannels[1])/float(Rate);
}


void ContinuousADC::fileTimeStr(char *str) const {
  float seconds = fileTime();
  float minutes = floor(seconds/60.0);
  seconds -= minutes*60;
  sprintf(str, "%02.0f:%02.0f", minutes, seconds);
}


void ContinuousADC::setMaxFileSamples(size_t samples) {
  FileMaxSamples = (samples/MajorSize)*MajorSize;
}


void ContinuousADC::setMaxFileTime(float secs) {
  setMaxFileSamples(secs*Rate*nchannels());
}


size_t ContinuousADC::maxFileSamples() const {
  return FileMaxSamples;
}


bool ContinuousADC::endWrite() {
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples ) {
    FileSamples = 0;
    return true;
  }
  else
    return false;
}


void ContinuousADC::setupChannels(uint8_t adc) {
  // translate to SC1A code:
  for(uint8_t i=0; i<NChannels[adc]; i++) {
    uint8_t sc1a_pin = 0;
    if ( adc == 0 )
      sc1a_pin = ADConv.channel2sc1aADC0[Channels[adc][i]];
    else if ( adc == 1 )
      sc1a_pin = ADConv.channel2sc1aADC1[Channels[adc][i]];
    SC1AChannels[adc][i] = (sc1a_pin & ADC_SC1A_CHANNELS) + ADC_SC1_AIEN;
  }
  if ( NChannels[adc] > 1 ) {
    // reorder:
    uint8_t temp = SC1AChannels[adc][0];
    for(uint8_t i=1; i<NChannels[adc]; i++)
      SC1AChannels[adc][i-1] = SC1AChannels[adc][i];
    SC1AChannels[adc][NChannels[adc]-1] = temp;
  }
  // configure for input:
  for(uint8_t i=0; i<NChannels[adc]; i++)
    pinMode(Channels[adc][i], INPUT);
}


void ContinuousADC::setupADC(uint8_t adc) {
  ADConv.adc[adc]->setAveraging(Averaging);
  ADConv.adc[adc]->setResolution(Bits);                  // bit depth of ADC
  ADConv.adc[adc]->setReference(ADC_REFERENCE::REF_3V3); // reference voltage
  ADConv.adc[adc]->setConversionSpeed(ConversionSpeed);  
  ADConv.adc[adc]->setSamplingSpeed(SamplingSpeed);
  ADConv.adc[adc]->enableDMA();                          // connect DMA and ADC
  ADConv.adc[adc]->stopTimer();
  ADConv.adc[adc]->startSingleRead(Channels[adc][0]);
  Bits = ADConv.adc[adc]->getResolution();

  /*
  analogReadResolution(Bits);
  //analogReference(DEFAULT);
  analogReadAveraging(Averaging);
  analogRead(Channels[adc][0]);
  */

  DataShift = DataBits - Bits;
}

    
void ContinuousADC::setupDMA(uint8_t adc) {
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
  for (int mi=NMajors-1; mi>=0; mi--) {
    DMABuffer[adc].source(adc==0?ADC0_RA:ADC1_RA);
    DMABuffer[adc].destinationCircular(&ADCBuffer[adc][mi*MajorSize], sizeof(uint16_t)*MajorSize);
    DMABuffer[adc].transferSize(sizeof(uint16_t));
    DMABuffer[adc].replaceSettingsOnCompletion(DMABuffer[adc]);
    DMABuffer[adc].TCD->DLASTSGA = (int32_t)&TCDs[adc][(mi+1)%NMajors];
    DMABuffer[adc].interruptAtCompletion();
    memcpy(&TCDs[adc][mi], DMABuffer[adc].TCD, sizeof(DMAChannel::TCD_t));
  }
  DMABuffer[adc].triggerAtHardwareEvent(adc==0?DMAMUX_SOURCE_ADC0:DMAMUX_SOURCE_ADC1); 
  DMABuffer[adc].attachInterrupt(adc==0?DMAISR0:DMAISR1);

  if ( NChannels[adc] > 1 ) {
    DMASwitch[adc].begin();
    DMASwitch[adc].sourceCircular(SC1AChannels[adc], NChannels[adc]);
    DMASwitch[adc].destination(adc==0?ADC0_SC1A:ADC1_SC1A); // this switches channels
    DMASwitch[adc].transferSize(1);
    DMASwitch[adc].triggerAtHardwareEvent(adc==0?DMAMUX_SOURCE_ADC0:DMAMUX_SOURCE_ADC1); 
  }
}


void ContinuousADC::isr(uint8_t adc) {
  // takes 38us! (=26kHz) for 256 samples
  size_t dmai = DMAIndex[adc]*MajorSize;
  DMAIndex[adc]++;
  if ( DMAIndex[adc] >= NMajors)
    DMAIndex[adc] = 0;
  // transform and copy dma buffer:
  size_t step = 1;
  if (ADCUse == 3)
    step = 2;
  for (size_t k=0; k<MajorSize; k++) {
    uint16_t val = ADCBuffer[adc][dmai++];
    val <<= DataShift;  // make 16 bit
    val += 0x8000;      // convert to signed int
    Buffer[BufferWrite[adc]] = val;
    BufferWrite[adc] += step;
    if (BufferWrite[adc] >= NBuffer)
      BufferWrite[adc] -= NBuffer;
  }
  DMACounter[adc]++;
  DMABuffer[adc].clearInterrupt();
  // TODO: check for buffer overrun! Only if we actually call writeData!
}


#if defined(ADC_USE_PDB)

void ContinuousADC::startPDB(uint32_t freq) {
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

  //                                   software trigger    enable PDB     PDB interrupt  continuous mode load immediately
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
  ContinuousADC::ADCC->isr(0);
}


void DMAISR1() {
  ContinuousADC::ADCC->isr(1);
}
