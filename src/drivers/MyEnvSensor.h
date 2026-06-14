#ifndef MYENVSENSOR_H
#define MYENVSENSOR_H
// インクルード
#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SHT31.h>
#include <Wire.h>

class MyEnvSensor
{

public:
  // メンバ
  Adafruit_SHT31 sht3x = Adafruit_SHT31(&Wire);
  Adafruit_BMP280 bme = Adafruit_BMP280(&Wire);

  float temp = 0.0;     // 温度
  float hum = 0.0;      // 温湿度
  float pressure = 0.0; // 気圧

  //  関数
  MyEnvSensor();
  ~MyEnvSensor();
  void initialize();
  void getValue();
};
#endif