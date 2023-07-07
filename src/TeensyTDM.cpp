#include <Arduino.h>
#include <TeensyTDM.h>

#if defined(KINETISK) || defined(__IMXRT1062__)

#include "utility/imxrt_hw.h"

// DMA buffer for 256 TDM frames:
#define TDM_FRAMES  256
DMAMEM __attribute__((aligned(32)))
static uint32_t TDMBuffer[TDM_FRAMES*8];

DMAChannel TeensyTDM::DMA(false);


TeensyTDM::TeensyTDM(volatile sample_t *buffer, size_t nbuffer) :
  DataBuffer(buffer, nbuffer) {
  setDataResolution(16);
  Rate = 0;
  NChannels = 0;
}


void TeensyTDM::setup(uint8_t nchannels, uint8_t bits, uint32_t rate) {
  NChannels = nchannels;  // make this a function in DataBuffer
  setResolution(bits);
  setRate(rate);
  if (Bits != 32)
    Serial.printf("TeensyTDM::setup() -> resolution of %ubits not supported.\n", Bits);
}


void TeensyTDM::setupTDM() {
  // this is config_tdm() from output_tdm.cpp of the Audio library
#if defined(KINETISK)
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
  //PLL:
  int fs = 48000; // XXX AUDIO_SAMPLE_RATE_EXACT;
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (fs * 256 * n1);

  double C = ((double)fs * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2);
  // clear SAI1_CLK register locations
  CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK))
    | CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4

  n1 = n1 / 2; //Double Speed for TDM

  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
    | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
    | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f

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


void TeensyTDM::ISR() {
  uint32_t daddr;
  const uint32_t *src;
  unsigned int i;
  unsigned int c;
  unsigned int nchannels;

  daddr = (uint32_t)(DMA.TCD->DADDR);
  DMA.clearInterrupt();

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
  nchannels = 4; // XXX should be NChannels!
  for (i=0; i < TDM_FRAMES/2; i++) {
    sample_t *slot = (sample_t *)src;
    for (c=0; c < nchannels; c++) {
      /*
      Buffer[Index++] = *slot++;
      if (Index >= NBuffer) {
	Index = 0;
	Cycles++;
      }
      */
    }
    src += 8;
  }
}


#endif


