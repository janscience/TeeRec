#include <Arduino.h>
#include <ContinuousADC.h>


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


float ContinuousADC::bufferTime() const {
  return float(NBuffer/nchannels())/Rate;
}


void ContinuousADC::pinAssignment() {
  const int npins = 23;
  int pins[npins] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
		     A13, A14, A15, A16, A17, A18, A19, A20, A21, A22};
  Serial.println("pin ADC0 ADC1");
  for (int k=0; k<npins; k++) {
    Serial.printf("A%-2d    %d    %d\n", k, ADConv.adc[0]->checkPin(pins[k]),
		  ADConv.adc[1]->checkPin(pins[k]));
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
  // report:
  Serial.println("ADC settings");
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  ADC0:       %dchannels\n", NChannels[0]);
  Serial.printf("  ADC1:       %dchannels\n", NChannels[1]);
  float bt = bufferTime();
  if (bt < 1.0)
    Serial.printf("  Buffer:     %.0fms\n", 1000.0*bt);
  else
    Serial.printf("  Buffer:     %.2fs\n", bt);
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
#endif
    Rate = ADConv.adc[0]->getTimerFrequency()/NChannels[0];
  }
  for (uint8_t adc=0; adc<2; adc++) {
    if ( (ADCUse & (adc+1)) == adc+1 ) {
      if ( (ADCUse & 3) != 3 ) {
	ADConv.adc[adc]->startTimer(Rate*NChannels[adc]);
	NVIC_DISABLE_IRQ(IRQ_PDB); // we don not need the PDB interrupt
	Rate = ADConv.adc[adc]->getTimerFrequency()/NChannels[adc];
      }
      if ( NChannels[adc] > 1 )
        DMASwitch[adc].enable();
      DMABuffer[adc].enable();
    }
  }
}


size_t ContinuousADC::frames(float time) const {
  return floor(time*Rate);
}


uint8_t ContinuousADC::adcs() const {
  return (ADCUse+1)/2;
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
  if (BufferRead >= last) {
    nwrite = NBuffer - BufferRead;
    if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
      nwrite = FileMaxSamples - FileSamples;
    nbytes = file.write((void *)&Buffer[BufferRead], sizeof(sample_t)*nwrite);
    samples0 = nbytes / sizeof(sample_t);
    BufferRead += samples0;
    if (BufferRead >= NBuffer)
      BufferRead -= NBuffer;
    FileSamples += samples0;
  }
  if ( FileMaxSamples > 0 && FileSamples >= FileMaxSamples )
    return samples0;
  nwrite = last - BufferRead;
  if (FileMaxSamples > 0 && nwrite > FileMaxSamples - FileSamples)
    nwrite = FileMaxSamples - FileSamples;
  nbytes = file.write((void *)&Buffer[BufferRead], sizeof(sample_t)*nwrite);
  samples1 = nbytes / sizeof(sample_t);
  BufferRead += samples1;
  if (BufferRead >= NBuffer)
    BufferRead -= NBuffer;
  FileSamples += samples1;
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
  if ( NChannels[adc] > 1 ) {
    // reorder:
    uint8_t temp = Channels[adc][0];
    for(uint8_t i=1; i<NChannels[adc]; i++)
      Channels[adc][i-1] = Channels[adc][i];
    Channels[adc][NChannels[adc]-1] = temp;
  }
  // translate to SC1A code:
  for(uint8_t i=0; i<NChannels[adc]; i++) {
    uint8_t sc1a_pin = 0;
    if ( adc == 0 )
      sc1a_pin = ADConv.channel2sc1aADC0[Channels[adc][i]];
    else if ( adc == 1 )
      sc1a_pin = ADConv.channel2sc1aADC1[Channels[adc][i]];
    SC1AChannels[adc][i] = (sc1a_pin & ADC_SC1A_CHANNELS) + ADC_SC1_AIEN;
  }
  // configure for input:
  for(uint8_t i=0; i<NChannels[adc]; i++)
    pinMode(Channels[adc][i], INPUT);
}


void ContinuousADC::setupADC(uint8_t adc) {
  ADConv.adc[adc]->setAveraging(1);
  ADConv.adc[adc]->setResolution(Bits);                                  // bit depth of ADC
  ADConv.adc[adc]->setReference(ADC_REFERENCE::REF_3V3);                 // reference voltage
  ADConv.adc[adc]->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);      
  ADConv.adc[adc]->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  ADConv.adc[adc]->enableDMA();                                          // connect DMA and ADC
  ADConv.adc[adc]->stopPDB();  
  ADConv.adc[adc]->startSingleRead(Channels[adc][0]);
  Bits = ADConv.adc[adc]->getResolution();
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

  DMABuffer[adc].begin(true);
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
    //    val = int16_t(0x4000*sin(6.2*BufferWrite[adc]/100));  // TEST
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
