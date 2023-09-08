#include "ControlPCM186x.h"

// #define DEBUG 1

// register addresses, MSB is page, LSB is register:
#define PCM186x_PGA_CH1L_REG 0x0001
#define PCM186x_PGA_CH1R_REG 0x0002
#define PCM186x_PGA_CH2L_REG 0x0003
#define PCM186x_PGA_CH2R_REG 0x0004
#define PCM186x_PGA_CONTROL_REG 0x0005
#define PCM186x_ADC1L_INPUT_SEL_REG 0x06
#define PCM186x_ADC1R_INPUT_SEL_REG 0x07
#define PCM186x_ADC2L_INPUT_SEL_REG 0x08
#define PCM186x_ADC2R_INPUT_SEL_REG 0x09
#define PCM186x_SEC_ADC_INPUT_SEL_REG 0x000A
#define PCM186x_I2S_FMT_REG 0x000B
#define PCM186x_I2S_TDM_OSEL_REG 0x000C
#define PCM186x_I2S_TX_OFFSET_REG 0x000D
#define PCM186x_RX_TDM_OFFSET_REG 0x000E
#define PCM186x_DPGA_VAL_CH1L_REG 0x000F
#define PCM186x_GPIO_FUNC_1_REG 0x0010
#define PCM186x_GPIO_FUNC_2_REG 0x0011
#define PCM186x_GPIO_DIR_1_REG 0x0012
#define PCM186x_GPIO_DIR_2_REG 0x0013
#define PCM186x_GPIO_INOUT_REG 0x0014
#define PCM186x_PULL_DOWN_DIS_REG 0x0015
#define PCM186x_DPGA_VAL_CH1R_REG 0x0016
#define PCM186x_DPGA_VAL_CH2L_REG 0x0017
#define PCM186x_DPGA_VAL_CH2R_REG 0x0018
#define PCM186x_PGA_CONTROL_MAPPING_REG 0x0019
#define PCM186x_DIGMIC_CTRL_REG 0x001A
#define PCM186x_I2S_RX_SYNC_REG 0x001B
#define PCM186x_CLK_MODE_REG 0x0020
#define PCM186x_CLK_DIV1_DSP_REG 0x0021
#define PCM186x_CLK_DIV2_DSP_REG 0x0022
#define PCM186x_CLK_DIV_ADC_REG 0x0023
#define PCM186x_CLK_DIV_PLL_SCK_REG 0x0025
#define PCM186x_CLK_DIV_SCK_BCK_REG 0x0026
#define PCM186x_CLK_DIV_BCK_LRCK_REG 0x0027
#define PCM186x_PLL_EN_REG 0x0028
#define PCM186x_PLL_P_REG 0x0029
#define PCM186x_PLL_R_REG 0x002A
#define PCM186x_PLL_J_REG 0x002B
#define PCM186x_PLL_D_LSB_REG 0x002C
#define PCM186x_PLL_D_MSB_REG 0x002D
#define PCM186x_SIGDET_CH_MODE_REG 0x0030
#define PCM186x_SIGDET_TRIG_MASK_REG 0x0031
#define PCM186x_SIGDET_STAT_REG 0x0032
#define PCM186x_SIGDET_LOSS_TIME_REG 0x0033
#define PCM186x_SIGDET_SCAN_TIME_REG 0x0034
#define PCM186x_SIGDET_INT_INTVL_REG 0x0036
// ... SIGDET ... quite some more registers ...
#define PCM186x_AUXADC_DATA_CTRL_REG 0x0058
#define PCM186x_AUXADC_DATA0_REG 0x0059
#define PCM186x_AUXADC_DATA1_REG 0x005A
#define PCM186x_INT_EN_REG 0x0060
#define PCM186x_INT_STAT_REG 0x0061
#define PCM186x_INT_PLS_REG 0x0062
#define PCM186x_PWRDN_CTRL_REG 0x0070
#define PCM186x_DSP_CTRL_REG 0x0071
#define PCM186x_DEV_STAT_REG 0x0072
#define PCM186x_FS_INFO_REG 0x0073
#define PCM186x_CURRENT_RATIO_REG 0x0074
#define PCM186x_CLK_ERROR_STAT_REG 0x0075
#define PCM186x_POWER_STAT_REG 0x0078

#define PCM186x_DSP2_MEM_MAP_REG 0x0101
#define PCM186x_MEM_ADDR_REG 0x0102
#define PCM186x_MEM_WDATA_0_REG 0x0104
#define PCM186x_MEM_WDATA_1_REG 0x0105
#define PCM186x_MEM_WDATA_2_REG 0x0106
#define PCM186x_MEM_WDATA_3_REG 0x0107
#define PCM186x_MEM_RDATA_0_REG 0x0108
#define PCM186x_MEM_RDATA_1_REG 0x0109
#define PCM186x_MEM_RDATA_2_REG 0x010A
#define PCM186x_MEM_RDATA_3_REG 0x010B

#define PCM186x_OSC_CTRL_REG 0x0312
#define PCM186x_MIC_BIAS_CTRL_REG 0x0315

#define PCM186x_PGA_ICI_REG 0xFD14

// virtual addresses of mixer coefficients
// (table 24 on page 69 of the data sheet):
#define PCM186x_MIX1_CH1L 0x00
#define PCM186x_MIX1_CH1R 0x01
#define PCM186x_MIX1_CH2L 0x02
#define PCM186x_MIX1_CH2R 0x03
#define PCM186x_MIX1_I2SL 0x04
#define PCM186x_MIX1_I2SR 0x05
#define PCM186x_MIX2_CH1L 0x06
#define PCM186x_MIX2_CH1R 0x07
#define PCM186x_MIX2_CH2L 0x08
#define PCM186x_MIX2_CH2R 0x09
#define PCM186x_MIX2_I2SL 0x0A
#define PCM186x_MIX2_I2SR 0x0B
#define PCM186x_MIX3_CH1L 0x0C
#define PCM186x_MIX3_CH1R 0x0D
#define PCM186x_MIX3_CH2L 0x0E
#define PCM186x_MIX3_CH2R 0x0F
#define PCM186x_MIX3_I2SL 0x10
#define PCM186x_MIX3_I2SR 0x11
#define PCM186x_MIX4_CH1L 0x12
#define PCM186x_MIX4_CH1R 0x13
#define PCM186x_MIX4_CH2L 0x14
#define PCM186x_MIX4_CH2R 0x15
#define PCM186x_MIX4_I2SL 0x16
#define PCM186x_MIX4_I2SR 0x17
#define PCM186x_LPF_B0 0x18
#define PCM186x_LPF_B1 0x19
#define PCM186x_LPF_B2 0x1A
#define PCM186x_LPF_A1 0x1B
#define PCM186x_LPF_A2 0x1C
#define PCM186x_HPF_B0 0x1D
#define PCM186x_HPF_B1 0x1E
#define PCM186x_HPF_B2 0x1F
#define PCM186x_HPF_A1 0x20
#define PCM186x_HPF_A2 0x21
#define PCM186x_LOSS_THRESHOLD 0x22
#define PCM186x_RESUME_THRESHOLD 0x23


ControlPCM186x::ControlPCM186x() :
  I2CBus(&Wire),
  I2CAddress(PCM186x_I2C_ADDR1),
  CurrentPage(10),
  PGALinked(false),
  NChannels(0),
  Bus(InputTDM::TDM1) {
}


ControlPCM186x::ControlPCM186x(uint8_t address) :
  I2CBus(&Wire),
  I2CAddress(address),
  CurrentPage(10),
  PGALinked(false),
  NChannels(0),
  Bus(InputTDM::TDM1) {
}


ControlPCM186x::ControlPCM186x(TwoWire &wire, uint8_t address) :
  I2CBus(&wire),
  I2CAddress(address),
  CurrentPage(10),
  PGALinked(false),
  NChannels(0),
  Bus(InputTDM::TDM1) {
}


ControlPCM186x::ControlPCM186x(TwoWire &wire, uint8_t address,
			       InputTDM::TDM_BUS bus) :
  I2CBus(&wire),
  I2CAddress(address),
  CurrentPage(10),
  PGALinked(false),
  NChannels(0),
  Bus(bus) {
}


bool ControlPCM186x::begin() {
  // power up:
  uint8_t val = 0x70;
  if (!write(PCM186x_PWRDN_CTRL_REG, val))
    return false;

  // setup clocks for BCK input slave PLL mode
  // (section 9.3.9.4.4 in data sheet):
  val = 0x01;           // CLKDET_EN enabled
  val += 0x02;          // DSP1_CLK_SRC = PLL
  val += 0x04;          // DSP2_CLK_SRC = PLL
  val += 0x08;          // ADC_CLK_SRC = PLL
  val += 0x00;          // MST_MODE = slave
  val += 0x20;          // MST_SCK_SRC = PLL
  if (!write(PCM186x_CLK_MODE_REG, val))
    return false;
  
  // enable filters:
  // PCM186x_DSP_CTRL_REG 0x71  plus page 1 registers?

  // disable micbias:
  // PCM186x_MIC_BIAS_CTRL_REG  0x0315
  
  return true;
}


bool ControlPCM186x::begin(uint8_t address) {
  return begin(Wire, address);
}


bool ControlPCM186x::begin(TwoWire &wire, uint8_t address) {
  I2CAddress = address;
  I2CBus = &wire;
  return begin();
}


void ControlPCM186x::setRate(InputTDM &tdm, uint32_t rate) {
  switch (rate) {
  case 8000:
  case 16000:
  case 48000:
  case 96000:
  case 192000:
    tdm.setRate(rate);
    break;
  case 24000:
    tdm.setRate(rate);
    tdm.downSample(2);
    break;
  default:
    Serial.printf("WARNING in ControlPCM186x::setRate(): invalid sampling rate of %dHz\n", rate);
    tdm.setRate(0);
    break;
  };
}
							     

ControlPCM186x::INPUT_CHANNELS ControlPCM186x::channel(OUTPUT_CHANNELS adc) {
  int ichan = 0x0100;
  if (adc == ADC1L)
    ichan = read(PCM186x_ADC1L_INPUT_SEL_REG);
  else if (adc == ADC1R)
    ichan = read(PCM186x_ADC1R_INPUT_SEL_REG);
  else if (adc == ADC2L)
    ichan = read(PCM186x_ADC2L_INPUT_SEL_REG);
  else if (adc == ADC2R)
    ichan = read(PCM186x_ADC2R_INPUT_SEL_REG);
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


const char *ControlPCM186x::channelStr(OUTPUT_CHANNELS adc) {
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


void ControlPCM186x::channels(char *chans, bool swaplr,
			      const char *prefix) {
  *chans = '\0';
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


bool ControlPCM186x::setupI2S(INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      bool inverted) {
  uint8_t fmt = 0x00;  // I2S
  DATA_BITS bits = BIT24;
  uint8_t val = fmt;   // FMT
  val |= bits << 2;    // TX_WLEN
  val |= bits << 6;    // RX_WLEN
  if (!write(PCM186x_I2S_FMT_REG, val))
    return false;
  if (!setChannel(ADC1L, channel1, inverted))
    return false;
  if (!setChannel(ADC1R, channel2, inverted))
    return false;
  NChannels = 2;
  return true;  
}


bool ControlPCM186x::setupI2S(INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      INPUT_CHANNELS channel3,
			      INPUT_CHANNELS channel4,
			      bool inverted) {
  // data format:
  uint8_t fmt = 0x00;   // I2S
  DATA_BITS bits = BIT24;
  uint8_t fval = fmt;   // FMT
  fval |= bits << 2;    // TX_WLEN
  fval |= bits << 6;    // RX_WLEN
  if (!write(PCM186x_I2S_FMT_REG, fval))
    return false;
  // enable DOUT2 on GPIO0:
  unsigned int val = read(PCM186x_GPIO_FUNC_1_REG);
  val &= ~0x07;
  val |= 0x05;
  if (!write(PCM186x_GPIO_FUNC_1_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, inverted))
    return false;
  if (!setChannel(ADC1R, channel2, inverted))
    return false;
  if (!setChannel(ADC2L, channel3, inverted))
    return false;
  if (!setChannel(ADC2R, channel4, inverted))
    return false;
  NChannels = 4;
  return true;  
}


void ControlPCM186x::setTDMChannelStr(InputTDM &tdm) {
  char cs[128];
  char tdmcs[128];
  tdm.channels(tdmcs);
  if (strlen(tdmcs) > 0) {
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
    sprintf(ps, "%d-", ++chipnum);
    char ccs[128];
    channels(ccs, tdm.swapLR(), ps);
    strcat(cs, ",");
    strcat(cs, ccs);
  }
  else
    channels(cs, tdm.swapLR());
  tdm.setChannelStr(cs);
}


bool ControlPCM186x::setupTDM(INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      bool offs, bool inverted) {
  // data format:
  uint8_t fmt = 0x03;   // TDM
  DATA_BITS bits = BIT32;
  uint8_t val = fmt;   // FMT
  val |= bits << 2;    // TX_WLEN
  val |= 0x10;         // TDM_LRCK_MODE
  val |= bits << 6;    // RX_WLEN
  if (!write(PCM186x_I2S_FMT_REG, val))
    return false;
  // number of ADCs:
  val = 0x00;        // TDM_OSEL: 2 channel TDM
  if (!write(PCM186x_I2S_TDM_OSEL_REG, val))
      return false;
  val = offs ? 0x80 : 0x00; // TX_TDM_OFFSET
  if (!write(PCM186x_I2S_TX_OFFSET_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, inverted))
    return false;
  if (!setChannel(ADC1R, channel2, inverted))
    return false;
  NChannels = 2;
  return true;  
}


bool ControlPCM186x::setupTDM(InputTDM &tdm,
			      INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      bool offs, bool inverted) {
  if (setupTDM(channel1, channel2, offs, inverted)) {
    tdm.setNChannels(Bus, offs ? 4 : 2);
    tdm.setResolution(32);
    setTDMChannelStr(tdm);
    return true;
  }
  return false;
}


bool ControlPCM186x::setupTDM(INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      INPUT_CHANNELS channel3,
			      INPUT_CHANNELS channel4,
			      bool offs, bool inverted) {
  // data format:
  uint8_t fmt = 0x03;   // TDM
  DATA_BITS bits = BIT32;
  uint8_t val = fmt;   // FMT
  val |= bits << 2;    // TX_WLEN
  val |= 0x10;         // TDM_LRCK_MODE
  val |= bits << 6;    // RX_WLEN
  if (!write(PCM186x_I2S_FMT_REG, val))
    return false;
  // number of ADCs:
  val = 0x01;           // TDM_OSEL: 4 channel TDM
  if (!write(PCM186x_I2S_TDM_OSEL_REG, val))
      return false;
  val = offs ? 0x80 : 0x00; // TX_TDM_OFFSET
  if (!write(PCM186x_I2S_TX_OFFSET_REG, val))
    return false;
  // input channels:
  if (!setChannel(ADC1L, channel1, inverted))
    return false;
  if (!setChannel(ADC1R, channel2, inverted))
    return false;
  if (!setChannel(ADC2L, channel3, inverted))
    return false;
  if (!setChannel(ADC2R, channel4, inverted))
    return false;
  NChannels = 4;
  return true;  
}


bool ControlPCM186x::setupTDM(InputTDM &tdm,
			      INPUT_CHANNELS channel1,
			      INPUT_CHANNELS channel2,
			      INPUT_CHANNELS channel3,
			      INPUT_CHANNELS channel4,
			      bool offs, bool inverted) {
  if (setupTDM(channel1, channel2, channel3, channel4, offs, inverted)) {
    tdm.setNChannels(Bus, offs ? 8 : 4);
    tdm.setResolution(32);
    setTDMChannelStr(tdm);
    return true;
  }
  return false;
}


bool ControlPCM186x::setChannel(OUTPUT_CHANNELS adc, INPUT_CHANNELS channel,
				bool inverted) {
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
      Serial.printf("ControlPCM186x: invalid channel %02x for ADCxL %0x2\n", channel, adc);
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
      Serial.printf("ControlPCM186x: invalid channel %02x for ADCxR %0x2\n", channel, adc);
      return false;
    }
  }
  // set bit 6 and 7:
  val += 0x40;    // RSV bit 6 always write 1
  if (inverted)
    val += 0x80;  // POL bit 7
  // set input channel for adc:
  if (adc == ADC1L) {
    if (!write(PCM186x_ADC1L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC1R) {
    if (!write(PCM186x_ADC1R_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC2L) {
    if (!write(PCM186x_ADC2L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == ADC2R) {
    if (!write(PCM186x_ADC2R_INPUT_SEL_REG, val))
      return false;
  }
  return true;
}


float ControlPCM186x::gain(OUTPUT_CHANNELS adc) {
  int igain = 0x0100;
  if (adc == ADC1L)
    igain = read(PCM186x_PGA_CH1L_REG);
  else if (adc == ADC1R)
    igain = read(PCM186x_PGA_CH1R_REG);
  else if (adc == ADC2L)
    igain = read(PCM186x_PGA_CH2L_REG);
  else if (adc == ADC2R)
    igain = read(PCM186x_PGA_CH2R_REG);
  if (igain > 0x00ff)
    return -999.0;
  // TODO: also check for mute!
  int8_t igainv = igain & 0x00ff;
  float gainv = (float)igainv;
  return 0.5*gainv;
}


void ControlPCM186x::gainStr(OUTPUT_CHANNELS adc, char *gains, float pregain) {
  *gains = '\0';
  float g = gain(adc);
  if (g < -500.0)
    return;
  sprintf(gains, "%.2fmV", 0.5*3300/pregain/pow(10.0, g/20.0));
}


bool ControlPCM186x::setGain(OUTPUT_CHANNELS adc, float gain, bool smooth) {
  // check gain:
  if (gain < -12.0) {
    Serial.printf("ControlPCM186x: invalid gain %g < 12dB\n", gain);
    return false;
  }
  if (gain > 40.0) {
    Serial.printf("ControlPCM186x: invalid gain %g > 40dB\n", gain);
    return false;
  }
  // smooth gain change:
  unsigned int val = read(PCM186x_PGA_CONTROL_REG);
  val &= ~0x80;
  if (smooth)
    val |= 0x80;    // SMOOTH
  if (!write(PCM186x_PGA_CONTROL_REG, val))
    return false;
  // set gains:
  int8_t igain = (int8_t)(2*gain);
  if (adc == ADCLR) {
    if (!PGALinked) {
      unsigned int val = read(PCM186x_PGA_CONTROL_REG);
      val |= 0x40;
      if (!write(PCM186x_PGA_CONTROL_REG, val))
	return false;
      PGALinked = true;
    }
    if (!write(PCM186x_PGA_CH1L_REG, igain))
      return false;
  }
  else {
    if (PGALinked) {
      unsigned int val = read(PCM186x_PGA_CONTROL_REG);
      val &= ~0x40;
      if (!write(PCM186x_PGA_CONTROL_REG, val))
	return false;
      PGALinked = false;
    }
    if (adc & ADC1L) {
      if (!write(PCM186x_PGA_CH1L_REG, igain))
	return false;
    }
    if (adc & ADC1R) {
      if (!write(PCM186x_PGA_CH1R_REG, igain))
	return false;
    }
    if (adc & ADC2L) {
      if (!write(PCM186x_PGA_CH2L_REG, igain))
	return false;
    }
    if (adc & ADC2R) {
      if (!write(PCM186x_PGA_CH2R_REG, igain))
	return false;
    }
  }
  return true;
}

  
float ControlPCM186x::gain() {
  return gain(ADC1L);
}
		    

void ControlPCM186x::gainStr(char *gains, float pregain) {
  gainStr(ADC1L, gains, pregain); 
}


bool ControlPCM186x::setGain(float gain) {
  return setGain(ADCLR, gain, false);
}


bool ControlPCM186x::setFilters(LOWPASS lowpass, bool highpass) {
  unsigned int val = read(PCM186x_DSP_CTRL_REG);
  val &= ~0x20;
  if (lowpass == IIR)
    val |= 0x02;    // FLT
  if (highpass)
    val |= 0x01;    // HPF_EN
  if (!write(PCM186x_DSP_CTRL_REG, val))
    return false;
  return true;
}


bool ControlPCM186x::mute(OUTPUT_CHANNELS adcs) {
  unsigned int val = read(PCM186x_DSP_CTRL_REG);
  val |= adcs;    // MUTE_CHX_Y
  if (!write(PCM186x_DSP_CTRL_REG, val))
    return false;
  return true;
}


bool ControlPCM186x::unmute(OUTPUT_CHANNELS adcs) {
  unsigned int val = read(PCM186x_DSP_CTRL_REG);
  val &= ~0x0f;
  val &= ~adcs;    // MUTE_CHX_Y
  if (!write(PCM186x_DSP_CTRL_REG, val))
    return false;
  return true;
}


bool ControlPCM186x::setMicBias(bool power, bool bypass) {
  unsigned int val = read(PCM186x_MIC_BIAS_CTRL_REG);
  val &= ~0x11;
  if (power)
    val |= 0x01;    // PDZ
  if (bypass)
    val |= 0x10;    // TERM
  if (!write(PCM186x_MIC_BIAS_CTRL_REG, val))
    return false;
  return true;
}


bool ControlPCM186x::powerdown() {
  unsigned int val = read(PCM186x_PWRDN_CTRL_REG);
  val &= ~0xF8;   // clear reserved bits
  val |= 0x70;    // write reserved bits
  val |= 0x04;    // set PWRDN
  if (!write(PCM186x_PWRDN_CTRL_REG, val))
    return false;
  return true;
}


bool ControlPCM186x::powerup() {
  unsigned int val = read(PCM186x_PWRDN_CTRL_REG);
  val &= ~0xF8;   // clear reserved bits
  val |= 0x70;    // write reserved bits
  val &= ~0x04;   // clear PWRDN
  if (!write(PCM186x_PWRDN_CTRL_REG, val))
    return false;
  return true;
}


void ControlPCM186x::printState() {
  Serial.print("DEV_STAT: ");
  unsigned int val = read(PCM186x_DEV_STAT_REG);
  val &= 0x0F;
  if (val == 0)
    Serial.println("power down");
  else if (val == 1)
    Serial.println("wait clock stable");
  else if (val == 2)
    Serial.println("release reset");
  else if (val == 3)
    Serial.println("stand-by");
  else if (val == 4)
    Serial.println("fade in");
  else if (val == 5)
    Serial.println("fade out");
  else if (val == 9)
    Serial.println("sleep");
  else if (val == 15)
    Serial.println("run");
  else
    Serial.println("reserved");

  Serial.print("FS_INFO: ");
  val = read(PCM186x_FS_INFO_REG);
  val &= 0x07;
  if (val == 0)
    Serial.println("out of range (low) or LRCK halt");
  else if (val == 1)
    Serial.println("8kHz");
  else if (val == 2)
    Serial.println("16kHz");
  else if (val == 3)
    Serial.println("32 to 48kHz");
  else if (val == 4)
    Serial.println("88.2kHz to 96kHz");
  else if (val == 5)
    Serial.println("176.4kHz to 192kHz");
  else if (val == 6)
    Serial.println("out of range (high)");
  else
    Serial.println("invalid sampling frequency");

  val = read(PCM186x_CURRENT_RATIO_REG);
  Serial.print("SCK_RATIO: ");
  unsigned int ratio = val & 0x07;
  if (ratio == 0)
    Serial.println("out of range (low) or SCK halt");
  else if (ratio == 1)
    Serial.println("128");
  else if (ratio == 2)
    Serial.println("256");
  else if (ratio == 3)
    Serial.println("384");
  else if (ratio == 4)
    Serial.println("512");
  else if (ratio == 5)
    Serial.println("768");
  else if (ratio == 6)
    Serial.println("out of range (high)");
  else
    Serial.println("invalid SCK ratio or LRCK halt");
  
  Serial.print("BCK_RATIO: ");
  ratio = (val >> 4) & 0x07;
  if (ratio == 0)
    Serial.println("out of range (low) or BCK halt");
  else if (ratio == 1)
    Serial.println("32");
  else if (ratio == 2)
    Serial.println("48");
  else if (ratio == 3)
    Serial.println("64");
  else if (ratio == 4)
    Serial.println("256");
  else if (ratio == 5)
    Serial.println("not assigned");
  else if (ratio == 6)
    Serial.println("out of range (high)");
  else
    Serial.println("invalid BCK ratio or LRCK halt");

  Serial.print("CLK_ERROR_STAT: ");
  val = read(PCM186x_CLK_ERROR_STAT_REG);
  if ((val & 0x01) > 0)
    Serial.print("SCK error ");
  if ((val & 0x02) > 0)
    Serial.print("BCK error ");
  if ((val & 0x04) > 0)
    Serial.print("LRCK error ");
  if ((val & 0x10) > 0)
    Serial.print("SCK halt ");
  if ((val & 0x20) > 0)
    Serial.print("BCK halt ");
  if ((val & 0x40) > 0)
    Serial.print("LRCK error ");
  Serial.println();

  Serial.print("POWER_STAT: ");
  val = read(PCM186x_POWER_STAT_REG);
  if ((val & 0x01) == 0)
    Serial.print("bad or missing LDO ");
  if ((val & 0x02) == 0)
    Serial.print("bad or missing AVDD ");
  if ((val & 0x04) == 0)
    Serial.print("bad or missing DVDD ");
  Serial.println();

  int8_t ival;
  ival = read(PCM186x_PGA_CH1L_REG);
  Serial.printf("PGA_CH1L: %5.1fdB\n", float(ival)/2);
  ival = read(PCM186x_PGA_CH1R_REG);
  Serial.printf("PGA_CH1R: %5.1fdB\n", float(ival)/2);
  ival = read(PCM186x_PGA_CH2L_REG);
  Serial.printf("PGA_CH2L: %5.1fdB\n", float(ival)/2);
  ival = read(PCM186x_PGA_CH2R_REG);
  Serial.printf("PGA_CH2R: %5.1fdB\n", float(ival)/2);
}


void ControlPCM186x::printRegisters() {
  for (uint8_t reg=0x01; reg<0x80; reg++) {
    uint8_t val = read(reg);
    Serial.printf("%02x %02x\n", reg, val);
  }
}


void ControlPCM186x::printDSPCoefficients() {
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


float ControlPCM186x::readCoefficient(uint8_t address) {
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
      Serial.printf("ControlPCM186x: readCoefficient() failed to go to page %02x, error = %02x\n", page, result);
      return -1.0;
    }
  }
  val = 0;
  for (int n=0; n < 10 && val == 0; n++) {
    val = read(0x0101);
    delay(10);
  }
  write(0x0102, address); // memory address for reading/writing coefficient
  write(0x0101, 0x02);    // bit 0 request=1/check=0 write, bit 1 request=1/check=0 read
  val = 0x02;             // check progress in reading
  for (int n=0; n < 10 && val == 0x02; n++) {
    val = read(0x0101);
    delay(10);
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


unsigned int ControlPCM186x::read(uint16_t address) {
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);
  uint8_t result;
  
  if (CurrentPage != page) {
    // switch page (three times, see page 68 of the data sheet):
    result = goToPage(page);
    result = goToPage(page);
    result = goToPage(page);
    if (result != 0) {
      Serial.printf("ControlPCM186x: read() failed to go to page %02x, error = %02x\n", page, result);
      return 0x0100;
    }
  }
  
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(reg);
  result = I2CBus->endTransmission(false);
  if (result != 0) {
    Serial.printf("ControlPCM186x: read() failed to write reg %02x on page %02x, error = %02x\n", reg, page, result);
    return 0x0200 + result;
  }
  if (I2CBus->requestFrom(I2CAddress, (uint8_t)1) < 1) {
    Serial.printf("ControlPCM186x: empty read() on page %02x reg %02x\n", page, reg);
    return 0x0400;
  }
  int val = I2CBus->read();
#ifdef DEBUG
    Serial.printf("ControlPCM186x: read page %02x, reg %02x, val %02x\n", page, reg, val);
#endif
  return val;
}


bool ControlPCM186x::write(uint16_t address, uint8_t val) {
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);
  uint8_t result;
  
#ifdef DEBUG
  Serial.printf("ControlPCM186x: write page %02x, reg %02x, val %02x\n", page, reg, val);
#endif

  if (CurrentPage != page) {
    // switch page (three times, see page 68 of the data sheet):
    result = goToPage(page);
    result = goToPage(page);
    result = goToPage(page);
    if (result != 0) {
      Serial.printf("ControlPCM186x: write() failed to go to page %02x, error = %02x\n", page, result);
      return false;
    }
  }
    
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(reg); delay(10);
  I2CBus->write(val); delay(10);
  result = I2CBus->endTransmission();
  if (result != 0) {
    Serial.printf("ControlPCM186x: write() failed, error = %02x\n", result);
    return false;
  }
  return true;
}


uint8_t ControlPCM186x::goToPage(byte page) {
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(0x00); // page register
  delay(10);
  I2CBus->write(page); // go to page
  delay(10);
  uint8_t result = I2CBus->endTransmission();
  if (result == 0)
    CurrentPage = page;
  return result;
}

