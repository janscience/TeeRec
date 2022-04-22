/*
  SenseBME280 - read temperature, humidity, and pressure from Bosch BME280.
  Created by Jan Benda, April 22nd, 2022.

  Wrapper for [SparkFun BME280 Arduino Library](https://github.com/sparkfun/SparkFun_BME280_Arduino_Library).

  Only supports forced mode and burst reading.

  [Bosch Sensortec Website](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)

  The [BME280
  Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf).
*/

#ifndef SenseBME280_h
#define SenseBME280_h


#include <Arduino.h>
#include <Wire.h>
#include <SparkFunBME280.h>
#include <Sensor.h>


class SenseBME280 : protected BME280 {

 public:

  // Do not initialize BME280 device yet.
  SenseBME280();
  
  // Initialize BME280 device with address (0x77 or 0x76) on default I2C bus.
  bool beginI2C(uint8_t address=0x77);

  // Initialize BME280 device with address (0x77 or 0x76) on I2C bus.
  bool beginI2C(TwoWire &wire, uint8_t address=0x77);
  
  // Initialize BME280 device on SPI bus with chip select pin.
  bool beginSPI(uint8_t cs_pin);
    
  // Return name of chip as string.
  const char* chip() const { return Chip; };

  // Return true if temperature sensor is available.
  bool available();

  // Request a temperature conversion.
  void request();

  // Recommended delay between a request() and read() in milliseconds.
  unsigned long delay() const;

  // Retrieve a temperature reading from the device.
  // You need to call request() at least delay() milliseconds before.
  void read();

  // The temperature in degrees celsius.
  // On error, return -INFINITY.
  float temperature() const { return Celsius; };

  // The relative humidity in percent.
  // On error, return -INFINITY.
  float humidity() const { return Humidity; };

  // The pressure in Pascal.
  // On error, return -INFINITY.
  float pressure() const { return Pressure; };

  
 private:

  void init();

  static const float NoValue = -INFINITY;

  char Chip[8];
  bool Measuring;
  float Celsius;
  float Humidity;
  float Pressure;
  
};


class TemperatureBME280 : public Sensor {

 public:

  TemperatureBME280(SenseBME280 *bme);

  // Return true if temperature sensor is available.
  virtual bool available() { return BME->available(); };

  // Return resolution of the temperature readings.
  virtual float resolution() const;

  // Report temperature device on serial monitor.
  virtual void report();

  // Request a temperature conversion.
  virtual void request() { BME->request(); };

  // Recommended delay between a request() and read() in milliseconds.
  virtual unsigned long delay() const { return BME->delay(); };

  // Retrieve a temperature reading from the device.
  virtual void read() { BME->read(); };

  // The temperature in degrees celsius.
  // On error, return -INFINITY.
  virtual float value() const { return BME->temperature(); };

  
 private:

  SenseBME280 *BME;

};


class HumidityBME280 : public Sensor {

 public:

  HumidityBME280(SenseBME280 *bme);

  // Return true if humidity sensor is available.
  virtual bool available() { return BME->available(); };

  // Return resolution of the humidity readings.
  virtual float resolution() const;

  // Report humidity device on serial monitor.
  virtual void report();

  // Request a humidity conversion.
  virtual void request() { BME->request(); };

  // Recommended delay between a request() and read() in milliseconds.
  virtual unsigned long delay() const { return BME->delay(); };

  // Retrieve a humidity reading from the device.
  virtual void read() { BME->read(); };

  // The relative humidity in percent.
  // On error, return -INFINITY.
  virtual float value() const { return BME->humidity(); };

  
 private:

  SenseBME280 *BME;

};


class PressureBME280 : public Sensor {

 public:

  PressureBME280(SenseBME280 *bme);

  // Return true if pressure sensor is available.
  virtual bool available() { return BME->available(); };

  // Return resolution of the pressure readings.
  virtual float resolution() const;

  // Report pressure device on serial monitor.
  virtual void report();

  // Request a pressure conversion.
  virtual void request() { BME->request(); };

  // Recommended delay between a request() and read() in milliseconds.
  virtual unsigned long delay() const { return BME->delay(); };

  // Retrieve a pressure reading from the device.
  virtual void read() { BME->read(); };

  // The pressure in Pascal.
  // On error, return -INFINITY.
  virtual float value() const { return BME->pressure(); };

  
 private:

  SenseBME280 *BME;

};



#endif
