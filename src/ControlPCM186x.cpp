#include "ControlPCM186x.h"


#define PCM186x_STATE_REG 0x0072
#define PCM186x_INFO_REG 0x0073
#define PCM186x_RATIO_REG 0x0074
#define PCM186x_CLOCK_ERR_STAT_REG 0x0075
#define PCM186x_VDD_REG 0x0078


ControlPCM186x::ControlPCM186x() :
  I2CBus(Wire),
  I2CAddress(PCM186x_I2C_ADDR) {
}


bool ControlPCM186x::begin(uint8_t address) {
  return begin(Wire, address);
}


bool ControlPCM186x::begin(TwoWire &wire, uint8_t address) {
  I2CAddress = address;
  I2CBus = wire;
}


void ControlPCM186x::printState() {
  Serial.print("STATE: ");
  unsigned int val = read(PCM186x_STATE_REG);
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

  Serial.print("INFO: ");
  unsigned int val = read(PCM186x_INFO_REG);
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

  unsigned int val = read(PCM186x_RATIO_REG);
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
  unsigned int ratio = (val >> 4) & 0x07;
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

  Serial.print("CLK_ERR_STAT: ");
  unsigned int val = read(PCM186x_CLOCK_ERR_STAT_REG);
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

  Serial.print("VDD: ");
  unsigned int val = read(PCM186x_VDD_REG);
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

  if (!goToPage(page)) {
    Serial.printf("ControlPCM186x: read() failed to go to page %02x\n", page);
    return 0x0100;
  }
  I2CBus.beginTransmission(I2CAddress);
  I2CBus.write(reg);
  uint8_t result = I2CBus.endTransmission();
  if (result != 0) {
    Serial.printf("ControlPCM186x: read() failed to write reg %02x on page %02x, error = %02x\n", reg, page, result);
    return 0x0200 + result;
  }
  if (I2CBus.requestFrom(I2CAddress, 1) < 1) {
    Serial.printf("ControlPCM186x: empty read() on page %02x reg %02x\n", page, reg);
    return 0x0400 + val;
  }
  int val = I2CBus.read();
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
    
  if (! goToPage(page))
    return false;
  
  I2CBus.beginTransmission(I2CAddress);
  I2CBus.write(reg); delay(10);
  I2CBus.write(val); delay(10);
  uint8_t result = I2CBus.endTransmission();
  if (result != 0) {
    Serial.printf("ControlPCM186x: Error in write() = %02x\n", result);
    return false;
  }
  return true;
}


bool ControlPCM186x::goToPage(byte page) {
  I2CBus.beginTransmission(I2CAddress);
  I2CBus.write(0x00); delay(10); // page register
  I2CBus.write(page); delay(10); // go to page
  uint8_t result = I2CBus.endTransmission();
  if (result != 0) {
    Serial.printf("ControlPCM186x: Error in goToPage() = %02x\n", result);
    if (result == 2) {
      // failed to transmit address
    } else if (result == 3) {
      // failed to transmit data
    }
    return false;
  }
  return true;
}

