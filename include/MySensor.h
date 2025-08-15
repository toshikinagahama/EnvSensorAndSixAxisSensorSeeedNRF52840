#ifndef MYSENSOR_H
#define MYSENSOR_H
// インクルード
#include <Arduino.h>
#include <nrf52840.h>
#include "MadgwickAHRS.h"
#include "LSM6DS3.h"
#include "Wire.h"

class MySensor
{
private:
  Madgwick filter;

public:
  // メンバ
  LSM6DS3 *IMU;
  float acc_x;
  float acc_y;
  float acc_z;
  float acc_comp;
  float gyr_x;
  float gyr_y;
  float gyr_z;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;

  //  関数
  MySensor();
  ~MySensor();
  void initialize();
  void getValue();
};
#endif