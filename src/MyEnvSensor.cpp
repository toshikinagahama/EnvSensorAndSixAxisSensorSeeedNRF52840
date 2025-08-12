#include "MyEnvSensor.h"
#include "global.h"

MyEnvSensor::MyEnvSensor()
{
}

MyEnvSensor::~MyEnvSensor()
{
}

void MyEnvSensor::initialize()
{
  Wire.begin();
  Serial.begin(115200);
  while (!this->bme.begin(0x76))
    ;
  while (!this->sht3x.begin(0x44))
    ;
}

void MyEnvSensor::getValue()
{

  this->tmp = sht3x.readTemperature();
  this->hum = sht3x.readHumidity();
}