#include <Device.h>


const char *Device::BusStrings[11] = {
    "unknown",
    "internal",
    "SingleWire",   // e.g. DHT22 from Aosong Electronics
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
  AddressStr(""),
  PinStr(""),
  Chip(""),
  Identifier(""),
  Available(false),
  NKeyVals(0) {
  setDeviceType("unknown");
}


void Device::setDeviceType(const char *devicetype) {
  strncpy(DeviceType, devicetype, MaxType);
  DeviceType[MaxType - 1] = '\0';
  add("Type", DeviceType);
}


Device::BUS Device::bus() const {
  return Bus;
}


void Device::setInternBus() {
  Bus = BUS::INTERN;
  add("Bus", BusStrings[bus()]);
}


void Device::setSingleWireBus(int pin) {
  Bus = BUS::SINGLEWIRE;
  Address = 0;
  Pin = pin;
  snprintf(PinStr, MaxPin, "%d", Pin);
  PinStr[MaxPin - 1] = '\0';
  strcpy(Identifier, busStr());
  sprintf(Identifier + strlen(Identifier), " %d", pin);
  add("Bus", BusStrings[bus()]);
  add("Pin", PinStr);
  add("Identifier", Identifier);
}


void Device::setOneWireBus(int pin) {
  Bus = BUS::ONEWIRE;
  Address = 0;
  Pin = pin;
  snprintf(PinStr, MaxPin, "%d", Pin);
  PinStr[MaxPin - 1] = '\0';
  add("Bus", BusStrings[bus()]);
  add("Pin", PinStr);
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
  if (strlen(busStr()) + 5 >= MaxStr)
    Serial.printf("ERROR in Device::setI2CBus(): number of characters %d of identifier longer than %d!\n", strlen(busStr()) + 5, MaxStr);
  snprintf(AddressStr, MaxPin, "%x", Address);
  AddressStr[MaxPin - 1] = '\0';
  strcpy(Identifier, busStr());
  snprintf(Identifier + strlen(Identifier), MaxStr - strlen(Identifier) - 1,
	   " %x", address);
  Identifier[MaxStr - 1] = '\0';
  add("Bus", BusStrings[bus()]);
  add("Address", AddressStr);
  add("Identifier", Identifier);
  
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
  snprintf(PinStr, MaxPin, "%d", Pin);
  PinStr[MaxPin - 1] = '\0';
  if (strlen(busStr()) + 3 >= MaxStr)
    Serial.printf("ERROR in Device::setSPIBus(): number of characters %d of identifier longer than %d!\n", strlen(busStr()) + 3, MaxStr);
  strcpy(Identifier, busStr());
  snprintf(Identifier + strlen(Identifier), MaxStr - strlen(Identifier) - 1,
	   " %d", cspin);
  Identifier[MaxStr - 1] = '\0';
  add("Bus", BusStrings[bus()]);
  add("Pin", PinStr);
  add("Identifier", Identifier);
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
  add("Chip", Chip);
}


const char* Device::identifier() const {
  return Identifier;
}


void Device::setIdentifier(const char *identifier) {
  strncpy(Identifier, identifier, MaxStr);
  Identifier[MaxStr - 1] = '\0';
  add("Identifier", Identifier);
}


bool Device::available() const {
  return Available;
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


int Device::add(const char *key, const char *value) {
  if ((key == 0) || (value == 0))
    return -1;
  int i = setValue(key, value);
  if (i >= 0)
    return i;
  if (NKeyVals >= MaxKeyVals)
    return -1;
  Keys[NKeyVals] = key;
  Values[NKeyVals] = value;
  return ++NKeyVals;
}


bool Device::setValue(size_t index, const char *value) {
  if (index >= NKeyVals)
    return false;
  if (value == 0)
    return false;
  Values[index] = value;
  return true;
}


int Device::setValue(const char *key, const char *value) {
  for (size_t k=0; k<NKeyVals; k++) {
    if (strcmp(Keys[k], key) == 0) {
      if (setValue(k, value))
	return k;
      else
	return -1;
    }
  }
  return -1;
}


void Device::write(Stream &stream, size_t indent, size_t indent_incr) const {
  if (available()) {
    stream.printf("%*s%s (%s):\n", indent, "", chip(), identifier());
    indent += indent_incr;
    size_t w = 0;
    for (size_t k=0; k<NKeyVals; k++) {
      if (w < strlen(Keys[k]))
	w = strlen(Keys[k]);
    }
    for (size_t k=0; k<NKeyVals; k++) {
      size_t kw = w >= strlen(Keys[k]) ? w - strlen(Keys[k]) : 0;
      stream.printf("%*s%s:%*s %s\n", indent, "", Keys[k], kw, "", Values[k]);
    }
  }
}
