#include "ControlPCM186x.h"

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



ControlPCM186x::ControlPCM186x() :
  I2CBus(&Wire),
  I2CAddress(PCM186x_I2C_ADDR) {
}


bool ControlPCM186x::begin(uint8_t address) {
  return begin(Wire, address);
}


bool ControlPCM186x::begin(TwoWire &wire, uint8_t address) {
  I2CAddress = address;
  I2CBus = &wire;

  // setup power:
  // PCM186x_PWRDN_CTRL_REG 0x0070

  // setup clocks for BCK input PLL slave mode:
  uint8_t val = 0x01;   // CLKDET_EN enabled
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


bool ControlPCM186x::setDataFormat(DATA_FMT fmt, DATA_BITS bits, bool offs) {
  uint8_t val = fmt;   // FMT
  val += bits << 2;    // TX_WLEN
  if (fmt == TDM)
    val += 0x10;       // TDM_LRCK_MODE
  val += bits << 6;    // RX_WLEN
  if (!write(PCM186x_I2S_FMT_REG, val))
    return false;
  if (fmt == TDM) {
    val = 0x01;        // TDM_OSEL: 4 channel TDM
    if (!write(PCM186x_I2S_TDM_OSEL_REG, val))
      return false;
  }
  if ((fmt == TDM) && offs) {
    val = 0x80;        // TX_TDM_OFFSET
    if (!write(PCM186x_I2S_TX_OFFSET_REG, val))
      return false;
  }
  else {
    val = 0;           // TX_TDM_OFFSET
    if (!write(PCM186x_I2S_TX_OFFSET_REG, val))
      return false;
  }
  return true;  
}


bool ControlPCM186x::setChannel(int adc, int channel, bool inverted) {
  // check and set channel:
  uint8_t val = 0;
  if (adc == PCM186x_CH1L || adc == PCM186x_CH2L) {
    if (channel == PCM186x_CH1L)
      val = 0x01;
    else if (channel == PCM186x_CH2L)
      val = 0x02;
    else if (channel == PCM186x_CH3L)
      val = 0x04;
    else if (channel == PCM186x_CH4L)
      val = 0x08;
    else {
      Serial.printf("ControlPCM186x: invalid channel %02x for ADCxL %0x2\n", channel, adc);
      return false;
    }
  }
  else if (adc == PCM186x_CH1R || adc == PCM186x_CH2R) {
    if (channel == PCM186x_CH1R)
      val = 0x01;
    else if (channel == PCM186x_CH2R)
      val = 0x02;
    else if (channel == PCM186x_CH3R)
      val = 0x04;
    else if (channel == PCM186x_CH4R)
      val = 0x08;
    else {
      Serial.printf("ControlPCM186x: invalid channel %02x for ADCxR %0x2\n", channel, adc);
      return false;
    }
  }
  // set bit 6 and 7:
  val += 0x20;   // bit 6 always write 1
  if (inverted)
    val += 0x40;
  // set input channel for adc:
  if (adc == PCM186x_CH1L) {
    if (!write(PCM186x_ADC1L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == PCM186x_CH1R) {
    if (!write(PCM186x_ADC1R_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == PCM186x_CH2L) {
    if (!write(PCM186x_ADC2L_INPUT_SEL_REG, val))
      return false;
  }
  else if (adc == PCM186x_CH2R) {
    if (!write(PCM186x_ADC2R_INPUT_SEL_REG, val))
      return false;
  }
  return true;
}


bool ControlPCM186x::setGain(int channel, float gain) {
  if (gain < -12.0) {
    Serial.printf("ControlPCM186x: invalid gain %g < 12dB\n", gain);
    return false;
  }
  if (gain > 40.0) {
    Serial.printf("ControlPCM186x: invalid gain %g > 40dB\n", gain);
    return false;
  }
  int8_t igain = (int8_t)(2*gain);
  // need to check the negative gains!
  Serial.printf("set gain %g to %02x\n", gain, igain);
  if (channel & PCM186x_CH1L) {
    if (!write(PCM186x_PGA_CH1L_REG, igain))
      return false;
  }
  if (channel & PCM186x_CH1R) {
    if (!write(PCM186x_PGA_CH1R_REG, igain))
      return false;
  }
  if (channel & PCM186x_CH2L) {
    if (!write(PCM186x_PGA_CH2L_REG, igain))
      return false;
  }
  if (channel & PCM186x_CH2R) {
    if (!write(PCM186x_PGA_CH2R_REG, igain))
      return false;
  }
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
}


unsigned int ControlPCM186x::read(uint16_t address) {
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);

  uint8_t result = goToPage(page);
  if (result != 0) {
    Serial.printf("ControlPCM186x: read() failed to go to page %02x, error = %02x\n", page, result);
    return 0x0100;
  }
  I2CBus->beginTransmission(I2CAddress);
  I2CBus->write(reg);
  result = I2CBus->endTransmission();
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

#ifdef DEBUG
    Serial.printf("ControlPCM186x: write page %02x, reg %02x, val %02x\n", page, reg, val);
#endif
    
  uint8_t result = goToPage(page);
  if (result != 0) {
    Serial.printf("ControlPCM186x: write() failed to go to page %02x, error = %02x\n", page, result);
    return false;
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
  I2CBus->write(0x00); delay(10); // page register
  I2CBus->write(page); delay(10); // go to page
  uint8_t result = I2CBus->endTransmission();
  return result;
}

