#ifndef MYSENSOR_H
#define MYSENSOR_H
// インクルード
#include <Arduino.h>
#include <nrf52840.h>
#include "LSM6DS3.h"
#include "Wire.h"

class MySensor
{

public:
  // メンバ
  LSM6DS3 *IMU;
  int16_t acc_x;
  int16_t acc_y;
  int16_t acc_z;
  int16_t gyr_x;
  int16_t gyr_y;
  int16_t gyr_z;

  //  関数
  MySensor();
  ~MySensor();
  void initialize();
  void getValue();
};
#endif