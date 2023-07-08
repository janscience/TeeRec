#include <Arduino.h>
#include <TeensyTDM.h>
#ifdef __IMXRT1062__
// For set_audioClock on T4.x
#include <utility/imxrt_hw.h>
#endif

#if defined(KINETISK) || defined(__IMXRT1062__)

// DMA buffer for 256 TDM frames:
#define TDM_FRAMES  256
DMAMEM __attribute__((aligned(32)))
static uint32_t TDMBuffer[TDM_FRAMES*8];

DMAChannel TeensyTDM::DMA(false);

TeensyTDM *TeensyTDM::TDM = 0;



TeensyTDM::TeensyTDM(volatile sample_t *buffer, size_t nbuffer) :
  DataBuffer(buffer, nbuffer) {
  TDM = this;
  setDataResolution(16);
  Rate = 0;
  NChannels = 0;
}


void TeensyTDM::setup(uint8_t nchannels, uint8_t bits, uint32_t rate) {
  bool success = true;
  if (bits != 32) {
    Serial.printf("TeensyTDM::setup() -> resolution of %ubits not supported.\n", bits);
    success = false;
  }
  if (!success)
    return;
  if (nchannels >= 8) {
    Serial.printf("TeensyTDM::setup() -> too many channels=%u.\n", nchannels);
    success = false;
  }
  NChannels = nchannels;  // make this a function in DataBuffer
  setResolution(bits);
  setRate(rate);
}


bool TeensyTDM::check() {
  if ( Rate < 1 ) {
    Serial.println("ERROR: no sampling rate specfied.");
    Rate = 0;
    NChannels = 0;
    return false;
  }
  if ( NBuffer < TDM_FRAMES*8 ) {
    Serial.printf("ERROR: no buffer allocated or buffer too small. NBuffer=%d\n", NBuffer);
    Rate = 0;
    NChannels = 0;
    return false;
  }
  if (bufferTime() < 0.1)
    Serial.printf("WARNING: buffer time %.0fms should be larger than 100ms!\n",
		  1000.0*bufferTime());
  if ( NChannels < 1 ) {
    Serial.println("ERROR: no channels specfied.");
    Rate = 0;
    NChannels = 0;
    return false;
  }
  return true;
}

  
void TeensyTDM::report() {
  float bt = bufferTime();
  char bts[20];
  if (bt < 1.0)
    sprintf(bts, "%.0fms", 1000.0*bt);
  else
    sprintf(bts, "%.2fs", bt);
  Serial.println("TDM settings:");
  Serial.printf("  rate:       %.1fkHz\n", 0.001*Rate);
  Serial.printf("  resolution: %dbits\n", Bits);
  Serial.printf("  channels:   %d\n", NChannels);
  Serial.printf("  Buffer:     %s\n", bts);
  Serial.println();
}


void TeensyTDM::begin() {
  setupTDM();
}


void TeensyTDM::end() {
}


void TeensyTDM::start() {
  setupDMA();
}


void TeensyTDM::stop() {
}


void TeensyTDM::setWaveHeader(WaveHeader &wave) const {
  DataWorker::setWaveHeader(wave);
  char cs[100];
  char *sp = cs;
  for (int c=0; c<NChannels; c++) {
    if (c > 0)
      *(sp++) = ',';
    sp += sprintf(sp, "%d", c);
  }
  *sp = '\0';
  wave.setChannels(cs);
}


void TeensyTDM::setupTDM() {
  // this is config_tdm() from output_tdm.cpp of the Audio library
  // merged with the setI2SFreq() function of Frank B from the Teensy forum.
#if defined(KINETISK)
  typedef struct {
    uint8_t mult;
    uint16_t div;
  } tmclk;

  const int numfreqs = 14;
  const int samplefreqs[numfreqs] = { 8000, 11025, 16000, 22050, 32000, 44100, (int)44117.64706 , 48000, 88200, (int)44117.64706 * 2, 96000, 176400, (int)44117.64706 * 4, 192000};

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
  while (I2S0_MCR & I2S_MCR_DUF) ;
  I2S0_MDR = I2S_MDR_FRACT((MCLK_MULT-1)) | I2S_MDR_DIVIDE((MCLK_DIV-1));

  bool rate_found = false;
  for (int f = 0; f < numfreqs; f++) {
    if (Rate == samplefreqs[f]) {
      while (I2S0_MCR & I2S_MCR_DUF);
      I2S0_MDR = I2S_MDR_FRACT((clkArr[f].mult - 1)) | I2S_MDR_DIVIDE((clkArr[f].div - 1));
      Rate = round(((float)F_PLL / 256.0) * clkArr[f].mult / clkArr[f].div); //return real freq
      rate_found = true;
    }
  }
  if (!rate_found) {
    Serial.printf("TeensyTDM::setupTDM() -> invalid sampling rate %d Hz.\n", Rate);
    Rate = 0;
    NChannels = 0;
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
  CCM_CCGR5 |= CCM_CCGR5_SAI1(CCM_CCGR_ON);

  // if either transmitter or receiver is enabled, do nothing
  if (I2S1_TCSR & I2S_TCSR_TE) return;
  if (I2S1_RCSR & I2S_RCSR_RE) return;
  // PLL:
  int fs = Rate;
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
//  Serial.printf("%6d : n1 = %d, n2 = %d, C = %12.6f ",freq,n1,n2,C);
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
//  Serial.printf("c0 = %d, c1 = %d, c2 = %d\n",c0,c1,c2);
  set_audioClock(c0, c1, c2, true);
  
  // clear SAI1_CLK register locations
  CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK))
    | CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4

  n1 = n1 / 2; // double speed for TDM

  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
    | CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1)  // &0x07
    | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1); // &0x3f

  // START//Added afterwards to make the SAI2 function at the desired frequency as well.
  CCM_CS2CDR = (CCM_CS2CDR & ~(CCM_CS2CDR_SAI2_CLK_PRED_MASK | CCM_CS2CDR_SAI2_CLK_PODF_MASK))
               | CCM_CS2CDR_SAI2_CLK_PRED(n1 - 1)  // &0x07
               | CCM_CS2CDR_SAI2_CLK_PODF(n2 - 1); // &0x3f)
  // END//Added afterwards to make the SAI2 function at the desired frequency as well.

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
#endif
}


void TeensyTDM::setupDMA() {
  // this is begin() from input_tdm.cpp of the Audio library
  DMA.begin(true); // Allocate the DMA channel first

  // TODO: should we set & clear the I2S_RCSR_SR bit here?
  setupTDM();
  
#if defined(KINETISK)
  CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0
  DMA.TCD->SADDR = &I2S0_RDR0;
  DMA.TCD->SOFF = 0;
  DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
  DMA.TCD->NBYTES_MLNO = 4;
  DMA.TCD->SLAST = 0;
  DMA.TCD->DADDR = TDMBuffer;
  DMA.TCD->DOFF = 4;
  DMA.TCD->CITER_ELINKNO = sizeof(TDMBuffer) / 4;
  DMA.TCD->DLASTSGA = -sizeof(TDMBuffer);
  DMA.TCD->BITER_ELINKNO = sizeof(TDMBuffer) / 4;
  DMA.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
  DMA.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
  DMA.enable();

  I2S0_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
  I2S0_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE; // TX clock enable, because sync'd to TX
  DMA.attachInterrupt(ISR);
#elif defined(__IMXRT1062__)
  CORE_PIN8_CONFIG  = 3;  //RX_DATA0
  IOMUXC_SAI1_RX_DATA0_SELECT_INPUT = 2;
  DMA.TCD->SADDR = &I2S1_RDR0;
  DMA.TCD->SOFF = 0;
  DMA.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
  DMA.TCD->NBYTES_MLNO = 4;
  DMA.TCD->SLAST = 0;
  DMA.TCD->DADDR = TDMBuffer;
  DMA.TCD->DOFF = 4;
  DMA.TCD->CITER_ELINKNO = sizeof(TDMBuffer) / 4;
  DMA.TCD->DLASTSGA = -sizeof(TDMBuffer);
  DMA.TCD->BITER_ELINKNO = sizeof(TDMBuffer) / 4;
  DMA.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
  DMA.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_RX);
  DMA.enable();

  I2S1_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
  DMA.attachInterrupt(ISR);	
#endif	
}


void TeensyTDM::TDMISR() {
  uint32_t daddr = (uint32_t)(DMA.TCD->DADDR);
  DMA.clearInterrupt();

  const uint32_t *src;
  if (daddr < (uint32_t)TDMBuffer + sizeof(TDMBuffer) / 2) {
    // DMA is receiving to the first half of the buffer
    // need to remove data from the second half
    src = &TDMBuffer[TDM_FRAMES*4];
  } else {
    // DMA is receiving to the second half of the buffer
    // need to remove data from the first half
    src = &TDMBuffer[0];
  }

#if IMXRT_CACHE_ENABLED >=1
  arm_dcache_delete((void*)src, sizeof(TDMBuffer) / 2);
#endif
  // copy from src into cyclic buffer:
  unsigned int nchannels = NChannels;
  for (unsigned int i=0; i < TDM_FRAMES/2; i++) {
    sample_t *slot = (sample_t *)src;
    for (unsigned int c=0; c < nchannels; c++) {
      Buffer[Index++] = *slot++;
      if (Index >= NBuffer) {
	Index = 0;
	Cycle++;
      }
    }
    src += 8;
  }
}


void TeensyTDM::ISR() {
  TDM->TDMISR();
}


#endif


