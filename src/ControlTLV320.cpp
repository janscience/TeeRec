#include "ControlTLV320.h"


const uint32_t ControlTLV320::SamplingRates[ControlTLV320::MaxSamplingRates] =
  {8000, 16000, 24000, 32000, 48000, 96000, 192000};
const uint8_t ControlTLV320::BitBits[4] = {16, 20, 24, 32};

const char *ControlTLV320::LowpassStrings[3] = {"linear", "low latency", "ultra-low latency"};
const char *ControlTLV320::OnOffStrings[2] = {"off", "on"};


// #define DEBUG 1

// register addresses page 0x00, MSB is page, LSB is register:
#define TLV320_SW_RESET_REG 0x0001
#define TLV320_SLEEP_CFG_REG 0x0002
#define TLV320_SHDN_CFG_REG 0x0005
#define TLV320_ASI_CFG0_REG 0x0007
#define TLV320_ASI_CFG1_REG 0x0008
#define TLV320_ASI_CFG2_REG 0x0009
#define TLV320_ASI_CH1_REG 0x000B
#define TLV320_ASI_CH2_REG 0x000C
#define TLV320_ASI_CH3_REG 0x000D
#define TLV320_ASI_CH4_REG 0x000E
#define TLV320_ASI_CH5_REG 0x000F
#define TLV320_ASI_CH6_REG 0x0010
#define TLV320_ASI_CH7_REG 0x0011
#define TLV320_ASI_CH8_REG 0x0012
#define TLV320_MST_CFG0_REG 0x0013
#define TLV320_MST_CFG1_REG 0x0014
#define TLV320_ASI_STS_REG 0x0015
#define TLV320_CLK_SRC_REG 0x0016
#define TLV320_PDMCLK_CFG_REG 0x001F
#define TLV320_PDMIN_CFG_REG 0x0020
#define TLV320_GPIO_CFG0_REG 0x0021
#define TLV320_GPO_CFG0_REG 0x0022
#define TLV320_GPO_CFG1_REG 0x0023
#define TLV320_GPO_CFG2_REG 0x0024
#define TLV320_GPO_CFG3_REG 0x0025
#define TLV320_GPO_VAL_REG 0x0029
#define TLV320_GPIO_MON_REG 0x002A
#define TLV320_GPI_CFG0_REG 0x002B
#define TLV320_GPI_CFG1_REG 0x002C
#define TLV320_GPI_MON_REG 0x002F
#define TLV320_INT_CFG_REG 0x0032
#define TLV320_INT_MASK0_REG 0x0033
#define TLV320_INT_LTCH0_REG 0x0036
#define TLV320_BIAS_CFG_REG 0x003B
#define TLV320_CH1_CFG0_REG 0x003C
#define TLV320_CH1_CFG1_REG 0x003D
#define TLV320_CH1_CFG2_REG 0x003E
#define TLV320_CH1_CFG3_REG 0x003F
#define TLV320_CH1_CFG4_REG 0x0040
#define TLV320_CH2_CFG0_REG 0x0041
#define TLV320_CH2_CFG1_REG 0x0042
#define TLV320_CH2_CFG2_REG 0x0043
#define TLV320_CH2_CFG3_REG 0x0044
#define TLV320_CH2_CFG4_REG 0x0045
#define TLV320_CH3_CFG0_REG 0x0046
#define TLV320_CH3_CFG1_REG 0x0047
#define TLV320_CH3_CFG2_REG 0x0048
#define TLV320_CH3_CFG3_REG 0x0049
#define TLV320_CH3_CFG4_REG 0x004A
#define TLV320_CH4_CFG0_REG 0x004B
#define TLV320_CH4_CFG1_REG 0x004C
#define TLV320_CH4_CFG2_REG 0x004D
#define TLV320_CH4_CFG3_REG 0x004E
#define TLV320_CH4_CFG4_REG 0x004F
#define TLV320_CH5_CFG2_REG 0x0052
#define TLV320_CH5_CFG3_REG 0x0053
#define TLV320_CH5_CFG4_REG 0x0054
#define TLV320_CH6_CFG2_REG 0x0057
#define TLV320_CH6_CFG3_REG 0x0058
#define TLV320_CH6_CFG4_REG 0x0059
#define TLV320_CH7_CFG2_REG 0x005C
#define TLV320_CH7_CFG3_REG 0x005D
#define TLV320_CH7_CFG4_REG 0x005E
#define TLV320_CH8_CFG2_REG 0x0061
#define TLV320_CH8_CFG3_REG 0x0062
#define TLV320_CH8_CFG4_REG 0x0063
#define TLV320_DSP_CFG0_REG 0x006B
#define TLV320_DSP_CFG1_REG 0x006C
#define TLV320_DRE_CFG0_REG 0x006D
#define TLV320_AGC_CFG0_REG 0x0070
#define TLV320_IN_CH_EN_REG 0x0073
#define TLV320_ASI_OUT_CH_EN_REG 0x0074
#define TLV320_PWR_CFG_REG 0x0075
#define TLV320_DEV_STS0_REG 0x0076
#define TLV320_DEV_STS1_REG 0x0077
#define TLV320_I2C_CHKSUM_REG 0x007E

// register addresses page 0x02, MSB is page, LSB is register:
#define TLV320_BQ1_N0_BYT1_REG 0x0208
// ... many more ...

// register addresses page 0x03, MSB is page, LSB is register:
#define TLV320_BQ7_N0_BYT1_REG 0x0308
// ... many more ...

// register addresses page 0x04, MSB is page, LSB is register:
#define TLV320_MIX1_CH1_BYT1_REG 0x0408
// ... many more ...
#define TLV320_IIR_N0_BYT1_REG 0x0448
// ... many more ...


ControlTLV320::ControlTLV320() :
  ControlTLV320(Wire, TLV320_I2C_ADDR1, InputTDM::TDM1) {
}


ControlTLV320::ControlTLV320(uint8_t address, InputTDM::TDM_BUS bus) :
  ControlTLV320(Wire, address, bus) {
}


ControlTLV320::ControlTLV320(TwoWire &wire, uint8_t address,
			     InputTDM::TDM_BUS bus) :
  Device(),
  I2CBus(&wire),
  I2CAddress(address),
  CurrentPage(10),
  Rate(0),
  Bits(BIT32),
  NChannels(0),
  Bus(bus),
  MaxAmplmV(1650.0),
  PGAGain(1.0),
  VolumeGain(1.0),
  PGAGainStr("0dB"),
  VolumeStr("0dB")
{
  for (uint8_t c=0; c<4; c++)
    UseChannel[c] = 0;
  setDeviceType("input");
  setI2CBus(wire, address);
  setChip("TLV320");
  // check I2C device presence:
  wire.begin();
  I2CBus->beginTransmission(I2CAddress);
  Available = (I2CBus->endTransmission() == 0);
}


bool ControlTLV320::begin() {
  Available = false;
  
  // check I2C device presence:
  I2CBus->beginTransmission(I2CAddress);
  if (I2CBus->endTransmission() != 0)
    return false;
  
  // power up:
  if (!powerup())
    return false;

  // setup slave mode with auto PLL:
  /* this is all default:
  unsigned int val = read(TLV320_MST_CFG0_REG);
  val &= ~0x08;  // FS_MODE: rate is multiple of 48kHz 
  val &= ~0x20;  // AUTO_MODE_PLL_DIS: PLL enabled 
  val &= ~0x40;  // AUTO_CLK_CFG: auto clock configuration enabled
  val &= ~0x80;  // MST_SLV_CFG: slave mode
  if (!write(TLV320_MST_CFG0_REG, val))
    return false;
  */

  Available = true;
  return true;
}


bool ControlTLV320::begin(uint8_t address) {
  return begin(Wire, address);
}


bool ControlTLV320::begin(TwoWire &wire, uint8_t address) {
  I2CAddress = address;
  I2CBus = &wire;
  return begin();
}


bool ControlTLV320::setRate(InputTDM &tdm, uint32_t rate) {
  Rate = rate;
  switch (rate) {
  case 8000:
  case 16000:
  case 24000:
  case 32000:
  case 48000:
  case 96000:
  case 192000:
  case 384000:
  case 768000:
    tdm.setRate(rate);
    return true;
    break;
  default:
    Serial.printf("WARNING in ControlTLV320::setRate(): invalid sampling rate of %dHz\n", rate);
    tdm.setRate(0);
    Rate = 0;
    return false;
    break;
  };
}


void ControlTLV320::setResolution(DATA_BITS bits) {
  Bits = bits;
}


const char *ControlTLV320::channelStr(uint8_t channel) {
  if (channel == 0 && UseChannel[channel] > 0) {
    if (UseChannel[channel] == 2)
      return (const char *)"IN1PM";
    else
      return (const char *)"IN1P";
  }
  else if (channel == 1 && UseChannel[channel] > 0) {
    if (UseChannel[channel] == 2)
      return (const char *)"IN2PM";
    else
      return (const char *)"IN2P";
  }
  else if (channel == 2 && UseChannel[channel] > 0) {
    if (UseChannel[channel] == 2)
      return (const char *)"IN3PM";
    else
      return (const char *)"IN3P";
  }
  else if (channel == 3 && UseChannel[channel] > 0) {
    if (UseChannel[channel] == 2)
      return (const char *)"IN4PM";
    else
      return (const char *)"IN4P";
  }
  else
    return (const char *)"NONE";
}


void ControlTLV320::channelsStr(char *chans, size_t nchans,
				const char *prefix) {
  // prepare channel string:
  *chans = '\0';
  size_t n = 6;      // strlen of a single channel name plus comma
  if (prefix != 0)
    n += strlen(prefix);
  n *= NChannels;
  if (n >= nchans)
    Serial.printf("ERROR in ControlTLV320::channelsStr(): size of chans (%d) too small for %d characters!\n", nchans, n);
  uint8_t i_chan = 0;
  for (uint8_t c=0; c<4; c++) {
    if (UseChannel[c] > 0) {
      i_chan++;
      if (prefix != 0)
	strcat(chans, prefix);
      strcat(chans, channelStr(c));
      if (i_chan < NChannels)
	strcat(chans, ",");
    }
  }
}


bool ControlTLV320::setupChannel(uint8_t channel, SOURCE source,
				 IMPEDANCE impedance,
				 COUPLING coupling, bool dre,
				 int8_t slot, uint8_t offs) {
  // check and set channel:
  uint8_t addr = 0;
  if (channel == 0)
    addr = TLV320_CH1_CFG0_REG;
  else if (channel == 1)
    addr = TLV320_CH2_CFG0_REG;
  else if (channel == 2)
    addr = TLV320_CH3_CFG0_REG;
  else if (channel == 3)
    addr = TLV320_CH4_CFG0_REG;
  else {
      Serial.printf("ERROR in ControlTLV320::setupChannel(): invalid channel %d\n", channel);
      return false;
  }
  /* TODO: ???
  // disable GPO on channel pin:
  if (!write(TLV320_GPO_CFG0_REG + channel, 0x00))
    return false;
  // disable GPI on channel pin:
  unsigned int val = 0x00;
  if (source == DIFFERENTIAL_INPUT) {
    val = read(TLV320_GPI_CFG0_REG + channel/2);
    val &= ~(0x07) << 4*(channel % 2);
    if (!write(TLV320_GPI_CFG0_REG + channel/2, val))
      return false;
  }
  */
  // select DRE:
  if (dre) {
    val = read(TLV320_DSP_CFG1_REG);
    val &= ~0x04;         // clear DRE_AGC_SEL
  }
  // configure channel:
  val = 0;
  if (dre)
    val |= 0x01;                  // DREEN
  val |= (impedance & 0x03) << 2; // IMP
  if (coupling == DC_CPL)
    val |= 0x10;                  // DC
  val |= (source & 0x03) << 5;    // INSRC
  val |= 0x80;                    // INTYP: Line input
  if (!write(addr, val))
    return false;
  // configure channel output:
  if (slot < 0) {
    slot = offs;
    for (uint8_t c=0; c<4; c++) {
      if (UseChannel[c] > 0)
	slot++;
    }
  }
  val = 0;
  val = slot & 0x3F;
  if (!write(TLV320_ASI_CH1_REG + channel, val))
    return false;
  if (UseChannel[channel] == 0)
    NChannels++;
  UseChannel[channel] = (source == DIFFERENTIAL_INPUT ? 2 : 1);
  return true;
}


bool ControlTLV320::setupChannels(uint8_t n_chans, SOURCE source,
				 IMPEDANCE impedance,
				 COUPLING coupling, bool dre,
				 int8_t slot, uint8_t offs) {
  if (n_chans > 4) {
    Serial.printf("ERROR in ControlTLV320::setupChannels(): too many channels %d requested.\n", n_chans);
    return false;
  }
  for (uint8_t c=0; c<n_chans; c++) {
    setupChannel(c, source, impedance, coupling, dre, slot, offs);
    if (slot >= 0)
      slot++;
  }
  return true;
}


bool ControlTLV320::setupI2S() {
  // data format:
  uint8_t fmt = 0x01;  // I2S
  uint8_t val = 0;
  //val &= ~0x01;        // TX_FILL, set for shared TDM bus!
  val |= fmt << 6;     // ASI_FORMAT
  val |= Bits << 4;    // ASI_WLEN
  if (Rate > 75000)
    val |= 0x02;       // TX_EDGE, see page 4 in sbaa383c.pdf
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  // TODO: only for the one chip directly connected to host:
  val = 0;
  val |= 0x20;         // TX_KEEPER: Bus keeper is always enabled
  val |= 0x80;         // TX_LSB: Transmit the LSB for the first half cycle and Hi-Z for the second half cycle 
  if (!write(TLV320_ASI_CFG1_REG, val))
    return false;
  return setActive();  
}


void ControlTLV320::updateTDMChannelStr(InputTDM &tdm) {
  char cs[InputTDM::MaxChannels];
  char tdmcs[InputTDM::MaxChannels];
  tdm.channelsStr(tdmcs, InputTDM::MaxChannels);
  if (strlen(tdmcs) > 0) {
    // already channels in tdm.channelsStr(), need to add prefix:
    bool prefix = true;
    int chipnum = 0;
    char *cp = cs;
    for (const char *sp=tdmcs; *sp != '\0'; sp++) {
      if (prefix) {
	if (*(sp+1) != '-') {
	  *(cp++) = '1';
	  *(cp++) = '-';
	  chipnum = 1;
	}
	else
	  chipnum = *(sp) - '0';
      }
      *(cp++) = *sp;
      prefix = false;
      if (*sp == ',')
	prefix = true;
    }
    *(cp++) = '\0';
    char ps[6];
    snprintf(ps, 6, "%d-", ++chipnum);
    char ccs[InputTDM::MaxChannels/2];
    channelsStr(ccs, InputTDM::MaxChannels/2, ps);
    strcat(cs, ",");
    strcat(cs, ccs);
  }
  else
    channelsStr(cs, InputTDM::MaxChannels);
  tdm.setChannelsStr(cs);
}


bool ControlTLV320::setupTDM() {
  // data format:
  uint8_t fmt = 0x00;  // TDM
  uint8_t val = 0;
  //val &= ~0x01;        // TX_FILL, set for shared TDM bus!
  val |= fmt << 6;     // ASI_FORMAT
  val |= Bits << 4;    // ASI_WLEN
  if (Rate > 75000)
    val |= 0x02;       // TX_EDGE, see page 4 in sbaa383c.pdf
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  // TODO: only for the one chip directly connected to host:
  val = 0;
  val |= 0x20;         // TX_KEEPER: Bus keeper is always enabled
  val |= 0x80;         // TX_LSB: Transmit the LSB for the first half cycle and Hi-Z for the second half cycle 
  if (!write(TLV320_ASI_CFG1_REG, val))
    return false;
  // TODO: conifgure channels here?!?
  return setActive();
}


bool ControlTLV320::setupTDM(InputTDM &tdm) {
  if (!setupTDM())
    return false;
  // configure TDM:
  tdm.setResolution(BitBits[Bits]);
  tdm.addNChannels(Bus, NChannels);
  updateTDMChannelStr(tdm);
  return true;
}


bool ControlTLV320::setActive() {
  // enable input channels:
  unsigned int val = 0x00;
  for (uint8_t c=0; c<4; c++) {
    if (UseChannel[c] > 0) {
      val |= 0x80 >> c;
    }
  }
  if (!write(TLV320_IN_CH_EN_REG, val))
    return false;
  // enable output channels and put unused channels into tristate mode:
  if (!write(TLV320_ASI_OUT_CH_EN_REG, val))
    return false;
  // power up:
  val = 0x60;    // power up ADC, PDM, and PLL
  if (!write(TLV320_PWR_CFG_REG, val))
    return false;
  return true;
}


float ControlTLV320::gainToDecibel(float gain) {
  return 20.0*log10(gain);
}


float ControlTLV320::gainFromDecibel(float level) {
  return pow(10.0, level/20.0);
}


float ControlTLV320::gainDecibel(uint8_t channel) {
  int8_t igain = 100;
  if (channel == 0)
    igain = read(TLV320_CH1_CFG1_REG) >> 2;
  else if (channel == 1)
    igain = read(TLV320_CH2_CFG1_REG) >> 2;
  else if (channel == 2)
    igain = read(TLV320_CH3_CFG1_REG) >> 2;
  else if (channel == 3)
    igain = read(TLV320_CH4_CFG1_REG) >> 2;
  if (igain > 42)
    return -999.0;
  float gainv = (float)igain;
  return gainv;
}


float ControlTLV320::setGainDecibel(uint8_t channel, float level) {
  // check level:
  if (level < 0.0) {
    Serial.printf("ERROR in ControlTLV320::setGainDecibel(): invalid level %g < 0dB\n", level);
    return NAN;
  }
  if (level > 42.0) {
    Serial.printf("ERROR in ControlTLV320::setGainDecibel(): invalid level %g > 42dB\n", level);
    return NAN;
  }
  // set level:
  int8_t igain = (uint8_t)(level) << 2;
  if (channel == 0) {
    if (!write(TLV320_CH1_CFG1_REG, igain))
      return NAN;
  }
  else if (channel == 1) {
    if (!write(TLV320_CH2_CFG1_REG, igain))
      return NAN;
  }
  else if (channel == 2) {
    if (!write(TLV320_CH3_CFG1_REG, igain))
      return NAN;
  }
  else if (channel == 3) {
    if (!write(TLV320_CH4_CFG1_REG, igain))
      return NAN;
  }
  else {
    Serial.printf("ERROR in ControlTLV320::setGainDecibel(): invalid channel %u >= 4\n", channel);
  }
  PGAGain = gainFromDecibel(float(igain));
  snprintf(PGAGainStr, 8, "%ddB", igain);
  PGAGainStr[7] = '\0';
  add("PGAGain", PGAGainStr);
  return float(igain);
}


float ControlTLV320::gain(uint8_t channel) {
  float level = gainDecibel(channel);
  return gainFromDecibel(level);
}


float ControlTLV320::setGain(uint8_t channel, float gain) {
  float level = gainToDecibel(gain);
  level = ControlTLV320::setGainDecibel(channel, level);
  return gainFromDecibel(level);
}

  
float ControlTLV320::gainDecibel() {
  return gainDecibel(0);
}
		    

float ControlTLV320::setGainDecibel(InputTDM &tdm, float level) {
  float clevel = 0;
  for (uint8_t channel=0; channel<4; channel++)
    clevel = setGainDecibel(channel, level);
  if (!isnan(clevel)) {
    tdm.setGain(MaxAmplmV/PGAGain/VolumeGain);
    return clevel;
  }
  return NAN;
}

  
float ControlTLV320::gain() {
  return gain(0);
}
		    

float ControlTLV320::setGain(InputTDM &tdm, float gain) {
  float cgain = 0;
  for (uint8_t channel=0; channel<4; channel++)
    cgain = setGain(channel, gain);
  if (!isnan(cgain)) {
    tdm.setGain(MaxAmplmV/PGAGain/VolumeGain);
    return cgain;
  }
  return NAN;
}

  
float ControlTLV320::volumeDecibel() {
  unsigned int val = read(TLV320_DSP_CFG1_REG);
  float level = 0.5*val - 100.5;
  return level;
}
		    

float ControlTLV320::setVolumeDecibel(InputTDM &tdm, float level) {
  // gang volume control across channels:
  unsigned int val = read(TLV320_DSP_CFG1_REG);
  val |= 0x80;    // DVOL_GANG
  if (!write(TLV320_DSP_CFG1_REG, val))
    return NAN;
  // set digital volume:
  if (level > 27.0)
    level = 27.0;
  if (level < -100.0)
    val = 0;
  else
    val = int(2.0*(level + 100.5));
  if (!write(TLV320_CH1_CFG2_REG, val))
    return NAN;
  level = 0.5*val - 100.5;
  VolumeGain = gainFromDecibel(level);
  snprintf(VolumeStr, 8, "%.1fdB", level);
  VolumeStr[7] = '\0';
  add("Volume", PGAGainStr);
  tdm.setGain(MaxAmplmV/PGAGain/VolumeGain);
  return level;
}


bool ControlTLV320::setSmoothGainChange(bool smooth) {
  unsigned int val = read(TLV320_DSP_CFG1_REG);
  val &= ~0x10;
  if (!smooth)
    val |= 0x10;    // DISABLE_SOFT_STEP
  if (!write(TLV320_DSP_CFG1_REG, val))
    return false;
  add("Smooth gain change", OnOffStrings[smooth]);
  return true;
}


bool ControlTLV320::setFilters(LOWPASS lowpass, HIGHPASS highpass) {
  unsigned int val = read(TLV320_DSP_CFG0_REG);
  val &= ~0x30;
  val |= lowpass << 4;
  val &= ~0x03;
  val |= highpass;
  if (!write(TLV320_DSP_CFG0_REG, val))
    return false;
  add("Lowpass", LowpassStrings[lowpass]);
  char hs[10];
  if (highpass == 0)
    strcpy(hs, "custom");
  else {
    if (Rate < 1.0)
      Serial.println("ERROR in ControlTLV320::setFilters(): setRate() should be called before setFilters()");
    float fac = 0;
    if (highpass == LOW_HP)
      fac = 0.00025;
    else if (highpass == MED_HP)
      fac = 0.002;
    else if (highpass == HIGH_HP)
      fac = 0.008;
    snprintf(hs, 10, "%.1fkHz", 1000*fac*Rate);
    hs[9] = '\0';
  }
  add("Highpass", hs);
  return true;
}


bool ControlTLV320::powerdown() {
  unsigned int val = read(TLV320_PWR_CFG_REG);
  val &= ~0x03;   // clear reserved bits
  val &= ~0xE0;   // clear MICBIAS, ADC, PDM, PLL
  if (!write(TLV320_PWR_CFG_REG, val))
    return false;

  val = 0;
  val &= ~0x01;   // clear SLEEP_ENZ
  if (!write(TLV320_SLEEP_CFG_REG, val))
    return false;

  Rate = 0;
  CurrentPage = 10;
  for (uint8_t c=0; c<4; c++)
    UseChannel[c] = 0;
  NChannels = 0;
  return true;
}


bool ControlTLV320::powerup() {
  unsigned int val = 0x01;  // software reset
  if (!write(TLV320_SW_RESET_REG, val))
    return false;
  Rate = 0;
  CurrentPage = 10;
  for (uint8_t c=0; c<4; c++)
    UseChannel[c] = 0;
  NChannels = 0;
  val = 0;
  val |= 0x01;    // set SLEEP_ENZ
  val |= 0x80;    // set AREG_SELECT to internal 1.8V supply
  if (!write(TLV320_SLEEP_CFG_REG, val))
    return false;
  delay(10);
  return true;
}


void ControlTLV320::printChannel(uint8_t channel) {
  uint8_t addr = 0;
  if (channel == 0)
    addr = TLV320_CH1_CFG0_REG;
  else if (channel == 1)
    addr = TLV320_CH2_CFG0_REG;
  else if (channel == 2)
    addr = TLV320_CH3_CFG0_REG;
  else if (channel == 3)
    addr = TLV320_CH4_CFG0_REG;
  else {
    Serial.printf("ERROR: invalid channel %d!\n", channel);
    return;
  }
  unsigned int val = read(addr);
  Serial.printf("CH%d_CFG0:\n", channel + 1);
  Serial.print("  Input source: ");
  unsigned int source = (val & 0x60) >> 5;
  if (source == 0)
    Serial.println("differential");
  else if (source == 1)
    Serial.println("single-ended");
  else if (source == 2)
    Serial.println("digital");
  else
    Serial.println("reserved");
  Serial.print("  Coupling    : ");
  if (val & 0x10)
    Serial.println("DC");
  else
    Serial.println("AC");
  Serial.print("  Impedance   : ");
  unsigned int imp = (val & 0x0C) >> 2;
  if (imp == 0)
    Serial.println("2.5kOhm");
  else if (imp == 1)
    Serial.println("10kOhm");
  else if (imp == 2)
    Serial.println("20kOhm");
  else
    Serial.println("reserved");
  Serial.print("  DRE and AGC : ");
  if (val & 0x01)
    Serial.println("enabled");
  else
    Serial.println("disabled");
  Serial.print("  Input type  : ");
  if (val & 0x80)
    Serial.println("line input");
  else
    Serial.println("microphone");
  // slot:
  val = read(TLV320_ASI_CH1_REG + channel);
  Serial.printf("  TDM slot    : %d\n", val & 0x3F);
  Serial.print("  Output line : ");
  if (val & 0x40)
    Serial.println("secondary output (GPIO1 or GPOx)");
  else
    Serial.println("primary output (SDOUT)");  
  // channel gain:
  if (channel == 0)
    addr = TLV320_CH1_CFG1_REG;
  else if (channel == 1)
    addr = TLV320_CH2_CFG1_REG;
  else if (channel == 2)
    addr = TLV320_CH3_CFG1_REG;
  else if (channel == 3)
    addr = TLV320_CH4_CFG1_REG;
  val = read(addr);
  Serial.print("  Gain        : ");
  uint8_t gain = val >> 2;
  if (gain <= 42)
    Serial.printf("%6.1fdB\n", float(gain));
  else
    Serial.println("reserved");
  // channel volume:
  val = read(TLV320_DSP_CFG1_REG);
  bool ganged = (val & 0x80);
  if (ganged)    // channels ganged
    addr = TLV320_CH1_CFG2_REG;
  else if (channel == 0)
    addr = TLV320_CH1_CFG2_REG;
  else if (channel == 1)
    addr = TLV320_CH2_CFG2_REG;
  else if (channel == 2)
    addr = TLV320_CH3_CFG2_REG;
  else if (channel == 3)
    addr = TLV320_CH4_CFG2_REG;
  val = read(addr);
  Serial.print("  Volume      : ");
  Serial.printf("%6.1fdB", 0.5*val - 100.5);
  if (ganged)
    Serial.println(" (ganged)");
  else
    Serial.println();
}


void ControlTLV320::printState() {
  for (uint8_t c=0; c<4; c++)
    printChannel(c);
  Serial.println("DSP_CFG0:");
  unsigned int val = read(TLV320_DSP_CFG0_REG);
  Serial.print("  Decimation filter: ");
  unsigned int flt = (val & 0x30) >> 4;
  if (flt == 0)
    Serial.println("linear");
  else if (flt == 1)
    Serial.println("low latency");
  else if (flt == 2)
    Serial.println("ultra-low latency");
  else if (flt == 3)
    Serial.println("reserved");
  Serial.print("  Highpass   filter: ");
  flt = (val & 0x03);
  if (flt == 0)
    Serial.println("custom");
  else if (flt == 1)
    Serial.println("cutoff 0.00025*fs");
  else if (flt == 2)
    Serial.println("cutoff 0.002*fs");
  else if (flt == 3)
    Serial.println("cutoff 0.008*fs");
  
  Serial.println("DEV_STS0:");
  val = read(TLV320_DEV_STS0_REG);
  Serial.printf("  %02X", val);
  uint8_t b = 0x80;
  for (uint8_t k=0; k<8; k++) {
    bool on = ((val & b) > 0);
    Serial.printf("  CH%d: %s\n", k + 1, OnOffStrings[on]);
    b >>= 1;
  }
  
  Serial.print("DEV_STS1: ");
  val = read(TLV320_DEV_STS1_REG);
  val >>= 5;
  if (val == 4)
    Serial.println("sleep mode");
  else if (val == 6)
    Serial.println("active, but all ADC or PDM channels turned off");
  else if (val == 7)
    Serial.println("active, at least one ADC or PDM channel turned on");
  else
    Serial.println("unknown");

  Serial.println("ASI_CFG0:");
  val = read(TLV320_ASI_CFG0_REG);
  Serial.print("  protocol     : ");
  uint8_t prot = (val >> 6) & 0x03;
  if (prot == 0)
    Serial.println("TDM");
  else if (prot == 1)
    Serial.println("I2S");
  else if (prot == 2)
    Serial.println("LJ");
  else if (prot == 3)
    Serial.println("reserved");
  Serial.print("  word length  : ");
  uint8_t wl = (val >> 4) & 0x03;
  if (wl == 0)
    Serial.println("16bit");
  else if (wl == 1)
    Serial.println("20bit");
  else if (wl == 2)
    Serial.println("24bit");
  else if (wl == 3)
    Serial.println("32bit");
  Serial.print("  unused cycles: ");
  if (val & 0x01)
    Serial.println("Hi-Z");
  else
    Serial.println("zero");
  
  Serial.println("ASI_STS:");
  Serial.print("  sampling rate      : ");
  val = read(TLV320_ASI_STS_REG);
  uint8_t rate = val & 0xF0;
  rate >>= 4;
  if (rate == 0)
    Serial.print("8kHz");
  else if (rate == 1)
    Serial.print("16kHz");
  else if (rate == 2)
    Serial.print("24kHz");
  else if (rate == 3)
    Serial.print("32kHz");
  else if (rate == 4)
    Serial.print("48kHz");
  else if (rate == 5)
    Serial.print("96kHz");
  else if (rate == 6)
    Serial.print("192kHz");
  else if (rate == 7)
    Serial.print("384kHz");
  else if (rate == 8)
    Serial.print("768kHz");
  else if (rate == 15)
    Serial.print("invalid");
  else
    Serial.print("reserved");
  Serial.println();
  
  Serial.print("  BCLK to FSYNC ratio: ");
  uint8_t ratio = val & 0x0F;
  if (ratio == 0)
    Serial.print("16");
  else if (ratio == 1)
    Serial.print("24");
  else if (ratio == 2)
    Serial.print("32");
  else if (ratio == 3)
    Serial.print("48");
  else if (ratio == 4)
    Serial.print("64");
  else if (ratio == 5)
    Serial.print("96");
  else if (ratio == 6)
    Serial.print("128");
  else if (ratio == 7)
    Serial.print("192");
  else if (ratio == 8)
    Serial.print("256");
  else if (ratio == 9)
    Serial.print("384");
  else if (ratio == 10)
    Serial.print("512");
  else if (ratio == 11)
    Serial.print("1024");
  else if (ratio == 12)
    Serial.print("2048");
  else if (ratio == 15)
    Serial.print("invalid");
  else
    Serial.print("reserved");
  Serial.println();
}


void ControlTLV320::printDSPCoefficients() {
  const uint8_t n_biquads = 5;
  const char biquad_names[n_biquads][4] = {"N0", "N1", "N2", "D1", "D2"};
  const uint8_t n_iirs = 3;
  const char iir_names[n_iirs][4] = {"N0", "N1", "D1"};

  Serial.println();
  Serial.println("Biquad filters:");
  uint32_t addr = TLV320_BQ1_N0_BYT1_REG;
  for (uint8_t b=1; b<=6; b++) {
    for (uint8_t n=0; n<n_biquads; n++) {
      uint32_t val = readCoefficient(addr);
      Serial.printf("  BQ%2d_%s: %08X", b, biquad_names[n], val);
      addr += 4;
    }
    Serial.println();
  }

  addr = TLV320_BQ7_N0_BYT1_REG;
  for (uint8_t b=8; b<=12; b++) {
    for (uint8_t n=0; n<n_biquads; n++) {
      uint32_t val = readCoefficient(addr);
      Serial.printf("  BQ%2d_%s: %08X", b, biquad_names[n], val);
      addr += 4;
    }
    Serial.println();
  }

  Serial.println("Mixer:");
  addr = TLV320_MIX1_CH1_BYT1_REG;
  for (uint8_t m=1; m<=4; m++) {
    for (uint8_t c=1; c<=4; c++) {
      uint32_t val = readCoefficient(addr);
      Serial.printf("  MIX%d_CH%d: %08X", m, c, val);
      addr += 4;
    }
    Serial.println();
  }

  addr = TLV320_IIR_N0_BYT1_REG;
  Serial.printf("IIR filter:\n");
  for (uint8_t i=0; i<n_iirs; i++) {
    uint32_t val = readCoefficient(addr);
    Serial.printf("  IIR_%s: %08X", iir_names[i], val);
    addr += 4;
  }
  Serial.println();
}


uint32_t ControlTLV320::readCoefficient(uint8_t address) {
  uint32_t frac = 0;
  unsigned int val;
  val = read(address++);  // bit[31:24]
  frac |= (val & 0xFF) << 24;
  val = read(address++);  // bit[23:16]
  frac |= (val & 0xFF) << 16;
  val = read(address++);  // bit[15:8]
  frac |= (val & 0xFF) << 8;
  val = read(address++);  // bit[7:0]
  frac |= (val & 0xFF);
  return frac;
}


unsigned int ControlTLV320::read(uint16_t address) {
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);
  uint8_t result;
  
  if (CurrentPage != page) {
    result = goToPage(page);
    if (result != 0) {
#ifdef DEBUG
      Serial.printf("ControlTLV320: read() failed to go to page %02x, error = %02x\n", page, result);
#endif
      return 0x0100;
    }
  }
  
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(reg);
  result = I2CBus->endTransmission(false);
  if (result != 0) {
#ifdef DEBUG
    Serial.printf("ControlTLV320: read() failed to write reg %02x on page %02x, error = %02x\n", reg, page, result);
#endif
    return 0x0200 + result;
  }
  if (I2CBus->requestFrom(I2CAddress, (uint8_t)1) < 1) {
    Serial.printf("ControlTLV320: empty read() on page %02x reg %02x\n", page, reg);
    return 0x0400;
  }
  int val = I2CBus->read();
#ifdef DEBUG
    Serial.printf("ControlTLV320: read page %02x, reg %02x, val %02x\n", page, reg, val);
#endif
  return val;
}


bool ControlTLV320::write(uint16_t address, uint8_t val) {
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);
  uint8_t result;
  
#ifdef DEBUG
  Serial.printf("ControlTLV320: write page %02x, reg %02x, val %02x\n", page, reg, val);
#endif

  if (CurrentPage != page) {
    result = goToPage(page);
    if (result != 0) {
#ifdef DEBUG
      Serial.printf("ControlTLV320: write() failed to go to page %02x, error = %02x\n", page, result);
#endif
      return false;
    }
  }
    
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(reg);
  delay(WriteDelay);
  I2CBus->write(val);
  delay(WriteDelay);
  result = I2CBus->endTransmission();
  if (result != 0) {
#ifdef DEBUG
    Serial.printf("ControlTLV320: write() failed, error = %02x\n", result);
#endif
    return false;
  }
  return true;
}


uint8_t ControlTLV320::goToPage(byte page) {
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(0x00); // page register
  delay(WriteDelay);
  I2CBus->write(page); // go to page
  delay(WriteDelay);
  uint8_t result = I2CBus->endTransmission();
  if (result == 0)
    CurrentPage = page;
  return result;
}

