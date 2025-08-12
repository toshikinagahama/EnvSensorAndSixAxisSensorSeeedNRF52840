#ifndef MYENVSENSOR_H
#define MYENVSENSOR_H
// インクルード
#include <Arduino.h>
#include <Adafruit_SHT31.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP280.h"
#include "global.h"

class MyEnvSensor
{

public:
  // メンバ
  Adafruit_SHT31 sht3x;
  Adafruit_BMP280 bme;
  float tmp = 0.0;
  float hum = 0.0;
  float pressure = 0.0;

  //  関数
  MyEnvSensor();
  ~MyEnvSensor();
  void initialize();
  void getValue();
};
#endif