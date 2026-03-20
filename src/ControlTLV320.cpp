#include "ControlTLV320.h"


const uint32_t ControlTLV320::SamplingRates[ControlTLV320::MaxSamplingRates] =
  {8000, 16000, 24000, 32000, 48000, 96000, 192000, 384000, 768000};

//const char *ControlTLV320::PolarityStrings[2] = {"non inverted", "inverted"};
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
#define TLV320_BQ1_NO_BYT1_REG 0x0208
// ... many more ...

// register addresses page 0x03, MSB is page, LSB is register:
#define TLV320_BQ7_NO_BYT1_REG 0x0308
// ... many more ...

// register addresses page 0x04, MSB is page, LSB is register:
#define TLV320_MIX1_CH1_BYT1_REG 0x0408
// ... many more ...
#define TLV320_IIR_NO_BYT1_REG 0x0408
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
  //NChannels(0),
  Bus(bus),
  GainStr("")
{
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

  /*
  // setup clocks for BCK input slave PLL mode
  // (section 9.3.9.4.4 in data sheet):
  val = 0x01;           // CLKDET_EN enabled
  val += 0x02;          // DSP1_CLK_SRC = PLL
  val += 0x04;          // DSP2_CLK_SRC = PLL
  val += 0x08;          // ADC_CLK_SRC = PLL
  val += 0x00;          // MST_MODE = slave
  val += 0x20;          // MST_SCK_SRC = PLL
  if (!write(TLV320_CLK_MODE_REG, val))
    return false;
  
  // enable filters:
  // TLV320_DSP_CTRL_REG 0x71  plus page 1 registers?

  // disable micbias:
  // TLV320_MIC_BIAS_CTRL_REG  0x0315
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


void ControlTLV320::setRate(InputTDM &tdm, uint32_t rate) {
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
    break;
  default:
    Serial.printf("WARNING in ControlTLV320::setRate(): invalid sampling rate of %dHz\n", rate);
    tdm.setRate(0);
    Rate = 0;
    break;
  };
}
							     

/*
ControlTLV320::INPUT_CHANNELS ControlTLV320::channel(OUTPUT_CHANNELS adc) {
  int ichan = 0x0100;
  if (adc == ADC1L)
    ichan = read(TLV320_ADC1L_INPUT_SEL_REG);
  else if (adc == ADC1R)
    ichan = read(TLV320_ADC1R_INPUT_SEL_REG);
  else if (adc == ADC2L)
    ichan = read(TLV320_ADC2L_INPUT_SEL_REG);
  else if (adc == ADC2R)
    ichan = read(TLV320_ADC2R_INPUT_SEL_REG);
  if (ichan > 0x00ff)
    return CHNONE;
  ichan &= 0x0f;
  INPUT_CHANNELS chan = CHNONE;
  if (adc == ADC1L || adc == ADC2L) {
    if (ichan == 0x01)
      chan = CH1L;
    else if (ichan == 0x02)
      chan = CH2L;
    else if (ichan == 0x04)
      chan = CH3L;
    else if (ichan == 0x08)
      chan = CH4L;
  }
  else if (adc == ADC1R || adc == ADC2R) {
    if (ichan == 0x01)
      chan = CH1R;
    else if (ichan == 0x02)
      chan = CH2R;
    else if (ichan == 0x04)
      chan = CH3R;
    else if (ichan == 0x08)
      chan = CH4R;
  }
  return chan;
}


const char *ControlTLV320::channelStr(OUTPUT_CHANNELS adc) {
  INPUT_CHANNELS chan = channel(adc);
  if (chan == CH1L)
    return (const char *)"CH1L";
  else if (chan == CH1R)
    return (const char *)"CH1R";
  else if (chan == CH2L)
    return (const char *)"CH2L";
  else if (chan == CH2R)
    return (const char *)"CH2R";
  else if (chan == CH3L)
    return (const char *)"CH3L";
  else if (chan == CH3R)
    return (const char *)"CH3R";
  else if (chan == CH4L)
    return (const char *)"CH4L";
  else if (chan == CH4R)
    return (const char *)"CH4R";
  else
    return (const char *)"NONE";
}


void ControlTLV320::channelsStr(char *chans, size_t nchans, bool swaplr,
				const char *prefix) {
  *chans = '\0';
  size_t n = 5;      // strlen of a single channel name plus comma
  if (prefix != 0)
    n += strlen(prefix);
  n *= 2;            // minimum of two channels
  if (NChannels > 2)
    n *= 2;          // four channels
  if (n >= nchans)
    Serial.printf("ERROR in ControlTLV320::channels(): size of chans (%d) too small for %d characters!\n", nchans, n);
  if (swaplr) {
    if (prefix != 0)
      strcat(chans, prefix);
    strcat(chans, channelStr(ADC1R));
    strcat(chans, ",");
    if (prefix != 0)
      strcat(chans, prefix);
    strcat(chans, channelStr(ADC1L));
    if (NChannels > 2) {
      strcat(chans, ",");
      if (prefix != 0)
	strcat(chans, prefix);
      strcat(chans, channelStr(ADC2R));
      strcat(chans, ",");
      if (prefix != 0)
	strcat(chans, prefix);
      strcat(chans, channelStr(ADC2L));
    }
  }
  else {
    if (prefix != 0)
      strcat(chans, prefix);
    strcat(chans, channelStr(ADC1L));
    strcat(chans, ",");
    if (prefix != 0)
      strcat(chans, prefix);
    strcat(chans, channelStr(ADC1R));
    if (NChannels > 2) {
      strcat(chans, ",");
      if (prefix != 0)
	strcat(chans, prefix);
      strcat(chans, channelStr(ADC2L));
      strcat(chans, ",");
      if (prefix != 0)
	strcat(chans, prefix);
      strcat(chans, channelStr(ADC2R));
    }
  }
}


bool ControlTLV320::setupI2S(INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     POLARITY polarity) {
  uint8_t fmt = 0x01;  // I2S
  DATA_BITS bits = BIT32;
  uint8_t val = 0;
  val |= fmt << 6;     // ASI_FORMAT
  val |= bits << 4;    // ASI_WLEN
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  if (!setChannel(ADC1L, channel1, polarity))
    return false;
  if (!setChannel(ADC1R, channel2, polarity))
    return false;
  NChannels = 2;
  return setActive();  
}


bool ControlTLV320::setupI2S(INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     INPUT_CHANNELS channel3,
			     INPUT_CHANNELS channel4,
			     POLARITY polarity) {
  // data format:
  uint8_t fmt = 0x01;  // I2S
  DATA_BITS bits = BIT32;
  uint8_t val = 0;
  val |= fmt << 6;     // ASI_FORMAT
  val |= bits << 4;    // ASI_WLEN
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  // enable DOUT2 on GPIO0:
  unsigned int val = read(TLV320_GPIO_FUNC_1_REG);
  val &= ~0x07;
  val |= 0x05;
  if (!write(TLV320_GPIO_FUNC_1_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, polarity))
    return false;
  if (!setChannel(ADC1R, channel2, polarity))
    return false;
  if (!setChannel(ADC2L, channel3, polarity))
    return false;
  if (!setChannel(ADC2R, channel4, polarity))
    return false;
  NChannels = 4;
  return setActive();  
}


void ControlTLV320::setTDMChannelStr(InputTDM &tdm) {
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
    channelsStr(ccs, InputTDM::MaxChannels/2, tdm.swapLR(), ps);
    strcat(cs, ",");
    strcat(cs, ccs);
  }
  else
    channelsStr(cs, InputTDM::MaxChannels, tdm.swapLR());
  tdm.setChannelsStr(cs);
}


bool ControlTLV320::setupTDM(INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     bool offs, POLARITY polarity) {
  // data format:
  uint8_t fmt = 0x00;  // TDM
  DATA_BITS bits = BIT32;
  uint8_t val = 0;
  val |= fmt << 6;     // ASI_FORMAT
  val |= bits << 4;    // ASI_WLEN
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  // number of ADCs:
  val = 0x00;        // TDM_OSEL: 2 channel TDM
  if (!write(TLV320_I2S_TDM_OSEL_REG, val))
      return false;
  val = offs ? 0x80 : 0x00; // TX_TDM_OFFSET
  if (!write(TLV320_I2S_TX_OFFSET_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, polarity))
    return false;
  if (!setChannel(ADC1R, channel2, polarity))
    return false;
  NChannels = 2;
  return setActive();  
}


bool ControlTLV320::setupTDM(InputTDM &tdm,
			     INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     bool offs, POLARITY polarity) {
  if (setupTDM(channel1, channel2, offs, polarity)) {
    if (offs)
      tdm.setNChannels(Bus, tdm.nchannels(Bus) + 2);
    else
      tdm.setNChannels(Bus, 2);
    tdm.setResolution(32);
    setTDMChannelStr(tdm);
    return true;
  }
  return false;
}


bool ControlTLV320::setupTDM(INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     INPUT_CHANNELS channel3,
			     INPUT_CHANNELS channel4,
			     bool offs, POLARITY polarity) {
  // data format:
  uint8_t fmt = 0x00;  // TDM
  DATA_BITS bits = BIT32;
  uint8_t val = 0;
  val |= fmt << 6;     // ASI_FORMAT
  val |= bits << 4;    // ASI_WLEN
  if (!write(TLV320_ASI_CFG0_REG, val))
    return false;
  // number of ADCs:
  val = 0x01;           // TDM_OSEL: 4 channel TDM
  if (!write(TLV320_I2S_TDM_OSEL_REG, val))
      return false;
  val = offs ? 0x80 : 0x00; // TX_TDM_OFFSET
  if (!write(TLV320_I2S_TX_OFFSET_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, polarity))
    return false;
  if (!setChannel(ADC1R, channel2, polarity))
    return false;
  if (!setChannel(ADC2L, channel3, polarity))
    return false;
  if (!setChannel(ADC2R, channel4, polarity))
    return false;
  NChannels = 4;
  return setActive();  
}


bool ControlTLV320::setupTDM(InputTDM &tdm,
			     INPUT_CHANNELS channel1,
			     INPUT_CHANNELS channel2,
			     INPUT_CHANNELS channel3,
			     INPUT_CHANNELS channel4,
			     bool offs, POLARITY polarity) {
  if (setupTDM(channel1, channel2, channel3, channel4, offs, polarity)) {
    if (offs)
      tdm.setNChannels(Bus, tdm.nchannels(Bus) + 4);
    else
      tdm.setNChannels(Bus, 4);
    tdm.setResolution(32);
    setTDMChannelStr(tdm);
    return true;
  }
  return false;
}


bool ControlTLV320::setChannel(OUTPUT_CHANNELS adc, INPUT_CHANNELS channel,
			       POLARITY polarity) {
  // check and set channel:
  uint8_t val = 0;
  if (adc == ADC1L || adc == ADC2L) {
    if (channel & CH1L)
      val |= 0x01;
    else if (channel & CH2L)
      val |= 0x02;
    else if (channel & CH3L)
      val |= 0x04;
    else if (channel & CH4L)
      val |= 0x08;
    else {
      Serial.printf("ControlTLV320: invalid channel %02x for ADCxL %0x2\n", channel, adc);
      return false;
    }
  }
  else if (adc == ADC1R || adc == ADC2R) {
    if (channel & CH1R)
      val |= 0x01;
    else if (channel & CH2R)
      val |= 0x02;
    else if (channel & CH3R)
      val |= 0x04;
    else if (channel & CH4R)
      val |= 0x08;
    else {
      Serial.printf("ControlTLV320: invalid channel %02x for ADCxR %0x2\n", channel, adc);
      return false;
    }
  }
  // set bit 6 and 7:
  val += 0x40;    // RSV bit 6 always write 1
  if (polarity == INVERTED)
    val += 0x80;  // POL bit 7
  // set input channel for adc:
  if (adc == ADC1L) {
    if (!write(TLV320_ADC1L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC1R) {
    if (!write(TLV320_ADC1R_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC2L) {
    if (!write(TLV320_ADC2L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC2R) {
    if (!write(TLV320_ADC2R_INPUT_SEL_REG, val))
      return false;
  }
  add("Polarity", PolarityStrings[polarity]);
  return true;
}


bool ControlTLV320::setActive() {
  // TODO: Enable channels P0_R115 (IN_CH_EN)
  
  unsigned int val = read(TLV320_PWR_CFG_REG);
  val &= ~0x03;   // clear reserved bits
  val |= 0x60;    // power up ADC, PDM, and PLL
  if (!write(TLV320_PWR_CFG_REG, val))
    return false;
}
*/

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
  // TODO: also check for mute!
  float gainv = (float)igain;
  return gainv;
}


float ControlTLV320::setGainDecibel(uint8_t channel, float level) {
  // check level:
  if (level < 0.0) {
    Serial.printf("ControlTLV320: invalid level %g < 0dB\n", level);
    return NAN;
  }
  if (level > 42.0) {
    Serial.printf("ControlTLV320: invalid level %g > 42dB\n", level);
    return NAN;
  }
  // set level:
  int8_t igain = (int8_t)(level) << 2;
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
    Serial.printf("ControlTLV320: invalid channel %u >= 4\n", channel);
  }
  snprintf(GainStr, 8, "%ddB", igain);
  GainStr[7] = '\0';
  add("Gain", GainStr);
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
    tdm.setGain(1650.0/gainFromDecibel(clevel));
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
    tdm.setGain(1650.0/cgain);
    return cgain;
  }
  return NAN;
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
      Serial.println("ControlTLV320: setRate() should be called before setFilters()");
    float fac = 0;
    if (highpass == LOWHP)
      fac = 0.00025;
    else if (highpass == MEDHP)
      fac = 0.002;
    else if (highpass == HIGHHP)
      fac = 0.008;
    snprintf(hs, 10, "%.1fkHz", 1000*fac*Rate);
    hs[9] = '\0';
  }
  add("Highpass", hs);
  return true;
}


/*
bool ControlTLV320::mute(OUTPUT_CHANNELS adcs) {
  unsigned int val = read(TLV320_DSP_CTRL_REG);
  val |= adcs;    // MUTE_CHX_Y
  if (!write(TLV320_DSP_CTRL_REG, val))
    return false;
  return true;
}


bool ControlTLV320::unmute(OUTPUT_CHANNELS adcs) {
  unsigned int val = read(TLV320_DSP_CTRL_REG);
  val &= ~0x0f;
  val &= ~adcs;    // MUTE_CHX_Y
  if (!write(TLV320_DSP_CTRL_REG, val))
    return false;
  return true;
}

*/

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
  return true;
}


bool ControlTLV320::powerup() {
  unsigned int val = 1;  // software rest
  if (!write(TLV320_SW_RESET_REG, val))
    return false;
  val = 0;
  val |= 0x01;    // set SLEEP_ENZ
  if (!write(TLV320_SLEEP_CFG_REG, val))
    return false;
  delay(2);
  val |= 0x80;    // set AREG_SELECT to internal 1.8V supply
  if (!write(TLV320_SLEEP_CFG_REG, val))
    return false;
  delay(10);
  val = 0;        // disable all channels
  if (!write(TLV320_IN_CH_EN_REG, val))
    return false;
  return true;
}


void ControlTLV320::printState() {
  Serial.println("DEV_STS0:");
  unsigned int val = read(TLV320_DEV_STS0_REG);
  unsigned int b = 1;
  for (unsigned int k=0; k<8; k++) {
    bool on = ((val & b) > 0);
    Serial.printf("  CH%d: %s\n", k + 1, OnOffStrings[on]);
    b <<= 1;
  }
  
  Serial.print("DEV_STS1: ");
  val = read(TLV320_DEV_STS1_REG);
  val &= 0xE0;
  val >>= 5;
  if (val == 4)
    Serial.println("sleep mode");
  else if (val == 6)
    Serial.println("active, but all ADC or PDM channels turned off");
  else if (val == 7)
    Serial.println("active, at least one ADC or PDM channel turned on");
  else
    Serial.println("unknown");
  
  Serial.println("ASI_STS:");
  Serial.print("  sampling rate: ");
  val = read(TLV320_ASI_STS_REG);
  unsigned int rate = val & 0xF0;
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
  unsigned int ratio = val & 0x0F;
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

/*
void ControlTLV320::printRegisters() {
  for (uint8_t reg=0x01; reg<0x80; reg++) {
    uint8_t val = read(reg);
    Serial.printf("%02x %02x\n", reg, val);
  }
}


void ControlTLV320::printDSPCoefficients() {
  const char names[][10] = {
    "MIX1_CH1L",
    "MIX1_CH1R",
    "MIX1_CH2L",
    "MIX1_CH2R",
    "MIX1_I2SL",
    "MIX1_I2SR",
    "MIX2_CH1L",
    "MIX2_CH1R",
    "MIX2_CH2L",
    "MIX2_CH2R",
    "MIX2_I2SL",
    "MIX2_I2SR",
    "MIX3_CH1L",
    "MIX3_CH1R",
    "MIX3_CH2L",
    "MIX3_CH2R",
    "MIX3_I2SL",
    "MIX3_I2SR",
    "MIX4_CH1L",
    "MIX4_CH1R",
    "MIX4_CH2L",
    "MIX4_CH2R",
    "MIX4_I2SL",
    "MIX4_I2SR" };
  
  for (uint8_t address=0x00; address<=0x17; address++) {
    float coeff = readCoefficient(address);
    Serial.printf("coefficient %s %02x = %g\n", names[address], address, coeff);
  }
}


float ControlTLV320::readCoefficient(uint8_t address) {
  float coeff = 0.0;
  uint32_t frac = 0;
  unsigned int val;
  uint8_t result;
  uint8_t page = 0x01;
  if (CurrentPage != page) {
    result = goToPage(page);
    result = goToPage(page);
    result = goToPage(page);
    if (result != 0) {
      Serial.printf("ControlTLV320: readCoefficient() failed to go to page %02x, error = %02x\n", page, result);
      return -1.0;
    }
  }
  val = 0;
  for (int n=0; n < 10 && val == 0; n++) {
    val = read(0x0101);
    delay(1);
  }
  write(0x0102, address); // memory address for reading/writing coefficient
  write(0x0101, 0x02);    // bit 0 request=1/check=0 write, bit 1 request=1/check=0 read
  val = 0x02;             // check progress in reading
  for (int n=0; n < 10 && val == 0x02; n++) {
    val = read(0x0101);
    delay(1);
  }
  val = read(0x0108);  // bit[23:16]
  coeff = (val & 0xf0) >> 4;
  frac = (val & 0x0f) << 16;
  val = read(0x0109);  // bit[15:8]
  frac |= val << 8;
  val = read(0x010A);  // bit[7:0]
  frac |= val;
  //Serial.printf("%d  %d\n", coeff, frac);
  coeff += frac/float(1 << 20);   // TODO: fix this!!!
  return coeff;
  // read(0x010B);  // read data from 24-bit memory, bit 7

  // write: 0x0104 bit[23:16], 0x0105 bit[15:8], 0x0106 bit[7:0],
  // 0x0107 bit 7 "write data to 24-bit memory"
}
*/


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

