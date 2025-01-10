#include <Device.h>


const char *Device::BusStrings[10] = {
    "unknown",
    "internal",
    "OneWire",
    "I2C-0",
    "I2C-1",
    "I2C-2",
    "I2C-3",
    "SPI-0"
    "SPI-1"
    "SPI-2"
};


Device::Device() :
  Bus(BUS::UNKNOWN),
  Address(0),
  Pin(-1),
  Chip(""),
  Identifier("") {
  setDeviceType("unknown");
}


void Device::setDeviceType(const char *devicetype) {
  strncpy(DeviceType, devicetype, MaxType);
  DeviceType[MaxType - 1] = '\0';
}


Device::BUS Device::bus() const {
  return Bus;
}


void Device::setOneWireBus(int pin) {
  Bus = BUS::ONEWIRE;
  Address = 0;
  Pin = pin;
}


void Device::setI2CBus(const TwoWire &wire, unsigned int address) {
  if (&wire == &Wire)
    Bus = BUS::I2C0;
#if WIRE_INTERFACES_COUNT >= 2
  else if (&wire == &Wire1)
    Bus = BUS::I2C1;
#endif
#if WIRE_INTERFACES_COUNT >= 3
  else if (&wire == &Wire2)
    Bus = BUS::I2C2;
#endif
#if WIRE_INTERFACES_COUNT >= 4
  else if (&wire == &Wire3)
    Bus = BUS::I2C3;
#endif
  Address = address;
  Pin = -1;
  strcpy(Identifier, busStr());
  sprintf(Identifier + strlen(Identifier), " %x", address);
}


void Device::setSPIBus(const SPIClass &spi, unsigned int cspin) {
  if (&spi == &SPI)
    Bus = BUS::SPI0;
#if defined(__MKL26Z64__)
  else if (&spi == &::SPI1)
    Bus = BUS::SPI1;
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__IMXRT1062__)
  else if (&spi == &::SPI1)
    Bus = BUS::SPI1;
  else if (&spi == &::SPI2)
    Bus = BUS::SPI2;
#endif
  Address = 0;
  Pin = cspin;
  strcpy(Identifier, busStr());
  sprintf(Identifier + strlen(Identifier), " %d", cspin);
}


unsigned int Device::address() const {
  return Address;
}


int Device::pin() const {
  return Pin;
}


const char* Device::chip() const {
  return Chip;
}


void Device::setChip(const char *chip) {
  strncpy(Chip, chip, MaxStr);
  Chip[MaxStr - 1] = '\0';
}


const char* Device::identifier() const {
  return Identifier;
}


void Device::setIdentifier(const char *identifier) {
  strncpy(Identifier, identifier, MaxStr);
  Identifier[MaxStr - 1] = '\0';
}


void Device::report(Stream &stream) {
  if (available()) {
    stream.printf("%-7s device %-12s", deviceType(), chip());
    if (bus() != BUS::UNKNOWN) {
      stream.printf(" on %-8s bus", busStr());
      if (address() != 0)
	stream.printf(" at address %4x", address());
      else if (pin() >= 0)
	stream.printf(" at pin     %4d", pin());
      else
	stream.printf("%16s", "");
    }
    else
      stream.printf("%32s", "");
    if (strlen(identifier()) > 0)
      stream.printf(" with ID %s", identifier());
    stream.println();
  }
}
