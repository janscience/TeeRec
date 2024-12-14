#include <Arduino.h>
#include <InputTDM.h>
#ifdef __IMXRT1062__
#include <utility/imxrt_hw.h>   // set_audioClock on T4.x
#endif

#if defined(KINETISK) || defined(__IMXRT1062__)

// DMA buffer for 256 TDM frames:
#define TDM_FRAME_SIZE  8
#define TDM_FRAMES  256
DMAMEM __attribute__((aligned(32)))
static uint32_t TDMBuffer[2][TDM_FRAMES*TDM_FRAME_SIZE];

DMAChannel InputTDM::DMA[2];

InputTDM *InputTDM::TDM = 0;



InputTDM::InputTDM(volatile sample_t *buffer, size_t nbuffer) :
  Input(buffer, nbuffer, TDM_FRAME_SIZE*TDM_FRAMES/2) {
  TDM = this;
  setDataResolution(16);
  Bits = 32;
  Rate = 0;
  NChannels = 0;
  DownSample = 1;
  SwapLR = false;
  Channels[0] = '\0';
  for (int bus=0; bus<2; bus++) {
    NChans[bus] = 0;
    DMACounter[bus] = 0;
    DataHead[bus] = 0;
  }
  TDMUse = 0;
}


void InputTDM::setResolution(uint8_t bits) {
  if (bits != 32) {
    Serial.printf("InputTDM::setResolution() -> resolution of %ubits not supported.\n", bits);
    Bits = 0;
  }
  else
    Bits = 32;
}


void InputTDM::downSample(uint8_t n) {
  if (n < 1)
    n = 1;
  DownSample = n;
}

  
void InputTDM::setNChannels(uint8_t nchannels) {
  setNChannels(TDM1, nchannels);
}

  
void InputTDM::setNChannels(TDM_BUS bus, uint8_t nchannels) {
  if (nchannels > 256/Bits) {
    Serial.printf("InputTDM::setNChannels() -> too many channels=%u.\n", nchannels);
    nchannels = 0;
  }
  if (nchannels == 0) {
    NChans[bus] = 0;
    TDMUse &= ~(1 << bus);
  }
  else {
    NChans[bus] = nchannels;
    TDMUse |= (1 << bus);
  }
  NChannels = 0;
  for (int k=0; k<2; k++)
    NChannels += NChans[k];
}


void InputTDM::channels(char *chans) const {
  strcpy(chans, Channels);
}

  
void InputTDM::setChannelStr(const char *cs) {
  strncpy(Channels, cs, 127);
}


void InputTDM::clearChannels(TDM_BUS bus) {
  NChans[bus] = 0;
  TDMUse &= ~(1 << bus);
  NChannels = 0;
  for (int k=0; k<2; k++)
    NChannels += NChans[k];
}


void InputTDM::clearChannels() {
  NChannels = 0;
  for (int k=0; k<2; k++)
    NChans[k] = 0;
  TDMUse = 0;
  Channels[0] = '\0';
}


bool InputTDM::swapLR() const {
  return SwapLR;
}


void InputTDM::setSwapLR(bool swap) {
  SwapLR = swap;
}


size_t InputTDM::counter(TDM_BUS bus) const {
  return DMACounter[bus];
}


bool InputTDM::check(uint8_t nchannels, Stream &stream) {
  if ( Rate < 1 ) {
    stream.println("ERROR: no sampling rate specfied.");
    Rate = 0;
    NChannels = 0;
    return false;
  }
  if ( NBuffer < TDM_FRAMES*8 ) {
    stream.printf("ERROR: no buffer allocated or buffer too small. NBuffer=%d\n", NBuffer);
    Rate = 0;
    NChannels = 0;
    return false;
  }
  if (bufferTime() < 0.1)
    stream.printf("WARNING: buffer time %.0fms should be larger than 100ms!\n",
		  1000.0*bufferTime());
  if ( NChannels < 1 ) {
    stream.println("ERROR: no channels specified.");
    Rate = 0;
    NChannels = 0;
    return false;
  }
  return Input::check(nchannels, stream);
}

  
void InputTDM::report(Stream &stream) {
  float bt = bufferTime();
  char bts[20];
  if (bt < 1.0)
    sprintf(bts, "%.0fms", 1000.0*bt);
  else
    sprintf(bts, "%.2fs", bt);
  float dt = DMABufferTime();
  char dts[20];
  sprintf(dts, "%.1fms", 1000.0*dt);
  stream.println("TDM settings:");
  stream.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  stream.printf("  resolution: %dbits\n", Bits);
  stream.printf("  channels:   %d\n", NChannels);
  stream.printf("  swap l/r:   %d\n", SwapLR);
  stream.printf("  buffer:     %s (%d samples)\n", bts, nbuffer());
  stream.printf("  DMA time:   %s\n", dts);
  stream.println();
}


void InputTDM::setWaveHeader(WaveHeader &wave) const {
  DataWorker::setWaveHeader(wave);
  if (strlen(Channels) > 0)
    wave.setChannels(Channels);
}


#if defined(KINETISK)
#if F_CPU == 180000000
  #define MCLK_SRC  0
#elif F_CPU == 216000000
  #define MCLK_SRC  1
#elif F_CPU == 240000000
  #define MCLK_SRC  0
#elif F_CPU == 256000000
  #define MCLK_SRC  1
#endif

#ifndef MCLK_SRC
#if F_CPU >= 20000000
  #define MCLK_SRC  3  // the PLL
#else
  #define MCLK_SRC  0  // system clock
#endif
#endif
#endif


void InputTDM::begin() {
  if (Bits == 0 || Rate == 0) {
    Serial.println("ERROR: InputTDM::begin() -> resultion and sampling rate not yet specified.");
    return;
  }
  
  // the following is config_tdm() from output_tdm.cpp of the Audio library
  // merged with the setI2SFreq() function of Frank B from the Teensy forum.
#if defined(KINETISK)
  typedef struct {
    uint8_t mult;
    uint16_t div;
  } tmclk;

  const int numfreqs = 14;
  const uint32_t samplefreqs[numfreqs] = { 8000, 11025, 16000, 22050, 32000, 44100, (uint32_t)44117.64706 , 48000, 88200, (uint32_t)44117.64706 * 2, 96000, 176400, (uint32_t)44117.64706 * 4, 192000};

#if (F_PLL==16000000)
  const tmclk clkArr[numfreqs] = {{16, 125}, {148, 839}, {32, 125}, {145, 411}, {64, 125}, {151, 214}, {12, 17}, {96, 125}, {151, 107}, {24, 17}, {192, 125}, {127, 45}, {48, 17}, {255, 83} };
#elif (F_PLL==72000000)
  const tmclk clkArr[numfreqs] = {{32, 1125}, {49, 1250}, {64, 1125}, {49, 625}, {128, 1125}, {98, 625}, {8, 51}, {64, 375}, {196, 625}, {16, 51}, {128, 375}, {249, 397}, {32, 51}, {185, 271} };
#elif (F_PLL==96000000)
  const tmclk clkArr[numfreqs] = {{8, 375}, {73, 2483}, {16, 375}, {147, 2500}, {32, 375}, {147, 1250}, {2, 17}, {16, 125}, {147, 625}, {4, 17}, {32, 125}, {151, 321}, {8, 17}, {64, 125} };
#elif (F_PLL==120000000)
  const tmclk clkArr[numfreqs] = {{32, 1875}, {89, 3784}, {64, 1875}, {147, 3125}, {128, 1875}, {205, 2179}, {8, 85}, {64, 625}, {89, 473}, {16, 85}, {128, 625}, {178, 473}, {32, 85}, {145, 354} };
#elif (F_PLL==144000000)
  const tmclk clkArr[numfreqs] = {{16, 1125}, {49, 2500}, {32, 1125}, {49, 1250}, {64, 1125}, {49, 625}, {4, 51}, {32, 375}, {98, 625}, {8, 51}, {64, 375}, {196, 625}, {16, 51}, {128, 375} };
#elif (F_PLL==168000000)
  const tmclk clkArr[numfreqs] = {{32, 2625}, {21, 1250}, {64, 2625}, {21, 625}, {128, 2625}, {42, 625}, {8, 119}, {64, 875}, {84, 625}, {16, 119}, {128, 875}, {168, 625}, {32, 119}, {189, 646} };
#elif (F_PLL==180000000)
  const tmclk clkArr[numfreqs] = {{46, 4043}, {49, 3125}, {73, 3208}, {98, 3125}, {183, 4021}, {196, 3125}, {16, 255}, {128, 1875}, {107, 853}, {32, 255}, {219, 1604}, {214, 853}, {64, 255}, {219, 802} };
#elif (F_PLL==192000000)
  const tmclk clkArr[numfreqs] = {{4, 375}, {37, 2517}, {8, 375}, {73, 2483}, {16, 375}, {147, 2500}, {1, 17}, {8, 125}, {147, 1250}, {2, 17}, {16, 125}, {147, 625}, {4, 17}, {32, 125} };
#elif (F_PLL==216000000)
  const tmclk clkArr[numfreqs] = {{32, 3375}, {49, 3750}, {64, 3375}, {49, 1875}, {128, 3375}, {98, 1875}, {8, 153}, {64, 1125}, {196, 1875}, {16, 153}, {128, 1125}, {226, 1081}, {32, 153}, {147, 646} };
#elif (F_PLL==240000000)
  const tmclk clkArr[numfreqs] = {{16, 1875}, {29, 2466}, {32, 1875}, {89, 3784}, {64, 1875}, {147, 3125}, {4, 85}, {32, 625}, {205, 2179}, {8, 85}, {64, 625}, {89, 473}, {16, 85}, {128, 625} };
#endif
  
  SIM_SCGC6 |= SIM_SCGC6_I2S;
  SIM_SCGC7 |= SIM_SCGC7_DMA;
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX;

  // if either transmitter or receiver is enabled, do nothing
  if (I2S0_TCSR & I2S_TCSR_TE) return;
  if (I2S0_RCSR & I2S_RCSR_RE) return;

  // enable MCLK output
  I2S0_MCR = I2S_MCR_MICS(MCLK_SRC) | I2S_MCR_MOE;

  bool rate_found = false;
  for (int f = 0; f < numfreqs; f++) {
    if (Rate*DownSample == samplefreqs[f]) {
      while (I2S0_MCR & I2S_MCR_DUF);
      I2S0_MDR = I2S_MDR_FRACT((clkArr[f].mult - 1)) | I2S_MDR_DIVIDE((clkArr[f].div - 1));
      Rate = round(((float)F_PLL / 256.0) * clkArr[f].mult / clkArr[f].div / DownSample); //return real freq
      rate_found = true;
    }
  }
  if (!rate_found) {
    Serial.printf("InputTDM::begin() -> invalid sampling rate %d Hz.\n", Rate);
    Rate = 0;
    return;
  }

  // configure transmitter
  I2S0_TMR = 0;
  I2S0_TCR1 = I2S_TCR1_TFW(4);
  I2S0_TCR2 = I2S_TCR2_SYNC(0) | I2S_TCR2_BCP | I2S_TCR2_MSEL(1)
    | I2S_TCR2_BCD | I2S_TCR2_DIV(0);
  I2S0_TCR3 = I2S_TCR3_TCE;
  I2S0_TCR4 = I2S_TCR4_FRSZ(7) | I2S_TCR4_SYWD(0) | I2S_TCR4_MF
    | I2S_TCR4_FSE | I2S_TCR4_FSD;
  I2S0_TCR5 = I2S_TCR5_WNW(31) | I2S_TCR5_W0W(31) | I2S_TCR5_FBT(31);

  // configure receiver (sync'd to transmitter clocks)
  I2S0_RMR = 0;
  I2S0_RCR1 = I2S_RCR1_RFW(4);
  I2S0_RCR2 = I2S_RCR2_SYNC(1) | I2S_TCR2_BCP | I2S_RCR2_MSEL(1)
    | I2S_RCR2_BCD | I2S_RCR2_DIV(0);
  I2S0_RCR3 = I2S_RCR3_RCE;
  I2S0_RCR4 = I2S_RCR4_FRSZ(7) | I2S_RCR4_SYWD(0) | I2S_RCR4_MF
    | I2S_RCR4_FSE | I2S_RCR4_FSD;
  I2S0_RCR5 = I2S_RCR5_WNW(31) | I2S_RCR5_W0W(31) | I2S_RCR5_FBT(31);

  // configure pin mux for 3 clock signals
  CORE_PIN23_CONFIG = PORT_PCR_MUX(6); // pin 23, PTC2, I2S0_TX_FS (LRCLK) - 44.1kHz
  CORE_PIN9_CONFIG  = PORT_PCR_MUX(6); // pin  9, PTC3, I2S0_TX_BCLK  - 11.2 MHz
  CORE_PIN11_CONFIG = PORT_PCR_MUX(6); // pin 11, PTC6, I2S0_MCLK - 22.5 MHz

#elif defined(__IMXRT1062__)
  if (TDMUse & (1 << TDM1)) {
    CCM_CCGR5 |= CCM_CCGR5_SAI1(CCM_CCGR_ON);

    // if either transmitter or receiver is enabled, do nothing
    if (I2S1_TCSR & I2S_TCSR_TE) return;
    if (I2S1_RCSR & I2S_RCSR_RE) return;
  }
  if (TDMUse & (1 << TDM2)) {
    CCM_CCGR5 |= CCM_CCGR5_SAI2(CCM_CCGR_ON);

    // if either transmitter or receiver is enabled, do nothing
    if (I2S2_TCSR & I2S_TCSR_TE) return;
    if (I2S2_RCSR & I2S_RCSR_RE) return;
  }
  
  // PLL:
  int fs = Rate*DownSample;
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  // Handle samplerates below 10K different
  int n1;
  if(fs > 10000) {
    n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  } else {
    n1 = 8;
  }
  int n2 = 1 + (24000000 * 27) / (fs * 256 * n1);
  if (n2 > 63) {
    // n2 must fit into a 6-bit field
    Serial.printf("ERROR: n2 exceeds 63 - %d\n",n2);
    return;
  }

  double C = ((double)fs * 256 * n1 * n2) / 24000000;
  //  Serial.printf("%6d : n1 = %d, n2 = %d, C = %12.6f ", freq, n1, n2, C);
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  //  Serial.printf("c0 = %d, c1 = %d, c2 = %d\n", c0, c1, c2);
  set_audioClock(c0, c1, c2, true);

  n1 = n1 / 2; // double speed for TDM
  
  if (TDMUse & (1 << TDM1)) {
    // clear SAI1_CLK register locations
    CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK))
      | CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4

    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
      | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1)  // &0x07
      | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1); // &0x3f

    IOMUXC_GPR_GPR1 = (IOMUXC_GPR_GPR1 & ~(IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_MASK))
      | (IOMUXC_GPR_GPR1_SAI1_MCLK_DIR | IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL(0));	//Select MCLK

    // configure transmitter
    int rsync = 0;
    int tsync = 1;

    I2S1_TMR = 0;
    I2S1_TCR1 = I2S_TCR1_RFW(4);
    I2S1_TCR2 = I2S_TCR2_SYNC(tsync) | I2S_TCR2_BCP | I2S_TCR2_MSEL(1)
      | I2S_TCR2_BCD | I2S_TCR2_DIV(0);
    I2S1_TCR3 = I2S_TCR3_TCE;
    I2S1_TCR4 = I2S_TCR4_FRSZ(7) | I2S_TCR4_SYWD(0) | I2S_TCR4_MF
      | I2S_TCR4_FSE | I2S_TCR4_FSD;
    I2S1_TCR5 = I2S_TCR5_WNW(31) | I2S_TCR5_W0W(31) | I2S_TCR5_FBT(31);

    I2S1_RMR = 0;
    I2S1_RCR1 = I2S_RCR1_RFW(4);
    I2S1_RCR2 = I2S_RCR2_SYNC(rsync) | I2S_TCR2_BCP | I2S_RCR2_MSEL(1)
      | I2S_RCR2_BCD | I2S_RCR2_DIV(0);
    I2S1_RCR3 = I2S_RCR3_RCE;
    I2S1_RCR4 = I2S_RCR4_FRSZ(7) | I2S_RCR4_SYWD(0) | I2S_RCR4_MF
      | I2S_RCR4_FSE | I2S_RCR4_FSD;
    I2S1_RCR5 = I2S_RCR5_WNW(31) | I2S_RCR5_W0W(31) | I2S_RCR5_FBT(31);

    CORE_PIN23_CONFIG = 3;  //1:MCLK
    CORE_PIN21_CONFIG = 3;  //1:RX_BCLK
    CORE_PIN20_CONFIG = 3;  //1:RX_SYNC
  }
  if (TDMUse & (1 << TDM2)) {
    // clear SAI2_CLK register locations
    CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI2_CLK_SEL_MASK))
      | CCM_CSCMR1_SAI2_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4

    CCM_CS2CDR = (CCM_CS2CDR & ~(CCM_CS2CDR_SAI2_CLK_PRED_MASK | CCM_CS2CDR_SAI2_CLK_PODF_MASK))
      | CCM_CS2CDR_SAI2_CLK_PRED(n1-1) // &0x07
      | CCM_CS2CDR_SAI2_CLK_PODF(n2-1); // &0x3f

    IOMUXC_GPR_GPR1 = (IOMUXC_GPR_GPR1 & ~(IOMUXC_GPR_GPR1_SAI2_MCLK3_SEL_MASK))
      | (IOMUXC_GPR_GPR1_SAI2_MCLK_DIR | IOMUXC_GPR_GPR1_SAI2_MCLK3_SEL(0));	//Select MCLK

    // configure transmitter
    int rsync = 1;
    int tsync = 0;

    I2S2_TMR = 0;
    I2S2_TCR1 = I2S_TCR1_RFW(4);
    I2S2_TCR2 = I2S_TCR2_SYNC(tsync) | I2S_TCR2_BCP | I2S_TCR2_MSEL(1)
      | I2S_TCR2_BCD | I2S_TCR2_DIV(0);
    I2S2_TCR3 = I2S_TCR3_TCE;
    I2S2_TCR4 = I2S_TCR4_FRSZ(7) | I2S_TCR4_SYWD(0) | I2S_TCR4_MF
      | I2S_TCR4_FSE | I2S_TCR4_FSD;
    I2S2_TCR5 = I2S_TCR5_WNW(31) | I2S_TCR5_W0W(31) | I2S_TCR5_FBT(31);

    // configure receiver (sync'd to transmitter clocks)
    I2S2_RMR = 0;
    I2S2_RCR1 = I2S_RCR1_RFW(4);
    I2S2_RCR2 = I2S_RCR2_SYNC(rsync) | I2S_TCR2_BCP | I2S_RCR2_MSEL(1)
      | I2S_RCR2_BCD | I2S_RCR2_DIV(0);
    I2S2_RCR3 = I2S_RCR3_RCE;
    I2S2_RCR4 = I2S_RCR4_FRSZ(7) | I2S_RCR4_SYWD(0) | I2S_RCR4_MF
      | I2S_RCR4_FSE | I2S_RCR4_FSD;
    I2S2_RCR5 = I2S_RCR5_WNW(31) | I2S_RCR5_W0W(31) | I2S_RCR5_FBT(31);

    CORE_PIN33_CONFIG = 2;  //2:MCLK
    CORE_PIN4_CONFIG  = 2;  //2:TX_BCLK
    CORE_PIN3_CONFIG  = 2;  //2:TX_SYNC
  }
#endif
}


void InputTDM::start() {
  reset();   // resets the buffer and consumers
             // (they also might want to know about Rate)
  // this is begin() from input_tdm.cpp of the Audio library
  for (int bus=0; bus < 2; bus++) {
    if (TDMUse & (1 << bus)) {
      DataHead[bus] = 0;
      DMACounter[bus] = 0;
      DMA[bus].begin(true); // Allocate the DMA channel first

      // TODO: should we set & clear the I2S_RCSR_SR bit here?
#if defined(KINETISK)
      CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0
      DMA[bus].TCD->SADDR = &I2S0_RDR0;
      DMA[bus].TCD->SOFF = 0;
      DMA[bus].TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
      DMA[bus].TCD->NBYTES_MLNO = 4;
      DMA[bus].TCD->SLAST = 0;
      DMA[bus].TCD->DADDR = TDMBuffer[bus];
      DMA[bus].TCD->DOFF = 4;
      DMA[bus].TCD->CITER_ELINKNO = sizeof(TDMBuffer[bus]) / 4;
      DMA[bus].TCD->DLASTSGA = -sizeof(TDMBuffer[bus]);
      DMA[bus].TCD->BITER_ELINKNO = sizeof(TDMBuffer[bus]) / 4;
      DMA[bus].TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
      DMA[bus].triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);

      I2S0_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
      I2S0_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE; // TX clock enable, because sync'd to TX
#elif defined(__IMXRT1062__)
      if (bus == TDM2) {
	CORE_PIN5_CONFIG = 2;  //2:RX_DATA0
	IOMUXC_SAI2_RX_DATA0_SELECT_INPUT = 0;
	DMA[bus].TCD->SADDR = &I2S2_RDR0;
      }
      else {
	CORE_PIN8_CONFIG  = 3;  //RX_DATA0
	IOMUXC_SAI1_RX_DATA0_SELECT_INPUT = 2;
	DMA[bus].TCD->SADDR = &I2S1_RDR0;
      }
      DMA[bus].TCD->SOFF = 0;
      DMA[bus].TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
      DMA[bus].TCD->NBYTES_MLNO = 4;
      DMA[bus].TCD->SLAST = 0;
      DMA[bus].TCD->DADDR = TDMBuffer[bus];
      DMA[bus].TCD->DOFF = 4;
      DMA[bus].TCD->CITER_ELINKNO = sizeof(TDMBuffer[bus]) / 4;
      DMA[bus].TCD->DLASTSGA = -sizeof(TDMBuffer[bus]);
      DMA[bus].TCD->BITER_ELINKNO = sizeof(TDMBuffer[bus]) / 4;
      DMA[bus].TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
      DMA[bus].triggerAtHardwareEvent(bus==TDM1?DMAMUX_SOURCE_SAI1_RX:DMAMUX_SOURCE_SAI2_RX);

      if (bus == TDM2) {
	I2S2_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
	I2S2_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE;
      }
      else
	I2S1_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
#endif
#if defined(__IMXRT1062__)
      DMA[bus].attachInterrupt(bus==TDM1?ISR0:ISR1);
#else
      DMA[bus].attachInterrupt(ISR0);
#endif
    }
  }
  
#if defined(__IMXRT1062__)
  if (TDMUse == 3)
    DataHead[TDM2] = NChans[TDM1];
#endif
  
  for (int bus=0; bus < 2; bus++) {
    if (TDMUse & (1 << bus))
      DMA[bus].enable();
  }

  if (TDMUse > 0)
    Input::start();
}


void InputTDM::stop() {
  for (int bus=0; bus < 2; bus++) {
    if (TDMUse & (1 << bus)) {
      DMA[bus].disable();
      DMA[bus].detachInterrupt();
    }
  }
  Input::stop();
}


void InputTDM::TDMISR(uint8_t bus) {
  uint32_t daddr = (uint32_t)(DMA[bus].TCD->DADDR);
  DMA[bus].clearInterrupt();
  
  const uint32_t *src;
  if (daddr < (uint32_t)TDMBuffer[bus] + sizeof(TDMBuffer[bus]) / 2) {
    // DMA is receiving to the first half of the buffer
    // need to remove data from the second half
    src = &TDMBuffer[bus][TDM_FRAMES*TDM_FRAME_SIZE/2];
  } else {
    // DMA is receiving to the second half of the buffer
    // need to remove data from the first half
    src = &TDMBuffer[bus][0];
  }

  if (DataHead[bus] >= NBuffer)
    DataHead[bus] -= NBuffer;

#if IMXRT_CACHE_ENABLED >= 1
  arm_dcache_delete((void*)src, sizeof(TDMBuffer[bus]) / 2);
#endif
  // copy from src into cyclic buffer:
  unsigned int nchannels = NChans[bus];
  sample_t buffer[nchannels];
  for (unsigned int i=0; i < TDM_FRAMES/2/DownSample; i++) {
    const sample_t *slot = (const sample_t *)src;
    for (unsigned int c=0; c < nchannels; c++) {
      slot++;
      unsigned int ci = c;
      if (SwapLR)
	ci += c%2 > 0 ? -1 : +1;
      buffer[ci] = *slot++;
    }
    memcpy((void *)&Buffer[DataHead[bus]], (void *)buffer, sizeof(buffer));
    DataHead[bus] += NChannels;
    if (DataHead[bus] >= NBuffer)
      DataHead[bus] -= NBuffer;
    // next frames:
    for (unsigned int j=0; j < DownSample; j++)
      src += 8;
  }
  DMACounter[bus]++;
#if defined(__IMXRT1062__)
  if (TDMUse == 3) {
    if (DMACounter[TDM1] == DMACounter[TDM2]) {
      if (DataHead[TDM1] < Index)
	Cycle++;
      Index = DataHead[TDM1];
    }
  } else
#endif
  {
    if (DataHead[bus] < Index)
      Cycle++;
    Index = DataHead[bus];
  }
}


void InputTDM::ISR0() {
  TDM->TDMISR(TDM1);
}


#if defined(__IMXRT1062__)
void InputTDM::ISR1() {
  TDM->TDMISR(TDM2);
}
#endif

#endif


