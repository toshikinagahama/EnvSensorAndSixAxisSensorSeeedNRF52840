#ifndef MYENVSENSOR_H
#define MYENVSENSOR_H
// インクルード
#include <Arduino.h>
#include <Wire.h>

#define MLX90614_ADDRESS 0x5A

class MyEnvSensor {

public:
  // メンバ
  float tmp_obj = 0.0; // 対象温度
  float tmp_env = 0.0; // 環境温度

  //  関数
  MyEnvSensor();
  ~MyEnvSensor();
  void initialize();
  void getValue();
  bool ReadFromMLX90614(char addr, char cmd, char *arry);
  bool GetTempFromMLX90614(char addr, char type, float *value);
};
#endif