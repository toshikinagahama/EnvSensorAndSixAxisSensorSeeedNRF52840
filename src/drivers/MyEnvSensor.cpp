#include "MyEnvSensor.h"
#include "MyGlobal.h"

MyEnvSensor::MyEnvSensor() {}

MyEnvSensor::~MyEnvSensor() {}

void MyEnvSensor::initialize()
{
  Serial.println("Initializing MyEnvSensor...");
  Wire.begin();
  if (!bme.begin(0x76))
  {
    Serial.println("Warning: Could not find a valid BMP280 sensor, check wiring!");
  }
  if (!sht3x.begin(0x44))
  {
    Serial.println("Warning: Could not find a valid SHT3X sensor, check wiring!");
  }

  // Wire.beginTransmission(MLX90614_ADDRESS);
  // Wire.write(0xFF);
  // Wire.write(0xE8);
  // Wire.endTransmission();

  // // 2. SCLラインをLowに落としてスリープを確定させる
  // // Wireライブラリを一度終了し、ピン操作を行う
  // Wire.end();
  // pinMode(D5, OUTPUT);
  // digitalWrite(D5, LOW);
}

void MyEnvSensor::getValue()
{
  this->pressure = bme.readPressure() / 100.0; // hPaに変換
  this->temp = sht3x.readTemperature();
  this->hum = sht3x.readHumidity();
}
