#include "MySensor.h"
#include "global.h"

MySensor::MySensor()
{
  this->IMU = new LSM6DS3(I2C_MODE, 0x6A);
}

MySensor::~MySensor()
{
}

void MySensor::initialize()
{
  filter.begin(10);                    // 10Hzで初期化
  this->IMU->settings.gyroRange = 500; // そんなに激しい動きはしないので、500dpsで十分
  this->IMU->settings.accelRange = 4;  // そんなに激しい動きはしないので、4gで十分
  while (this->IMU->begin() != 0)
  {
  }
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, 0x8C);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x4A);
  this->IMU->writeRegister(0x17, 0x04);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL7_G, 0x00);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL8_XL, 0x09);
}

void MySensor::getValue()
{
  this->acc_x = this->IMU->readFloatAccelX();
  this->acc_y = this->IMU->readFloatAccelY();
  this->acc_z = this->IMU->readFloatAccelZ();
  this->gyr_x = this->IMU->readFloatGyroX();
  this->gyr_y = this->IMU->readFloatGyroX();
  this->gyr_z = this->IMU->readFloatGyroX();
  filter.updateIMU(this->gyr_x, this->gyr_y, this->gyr_z, this->acc_x, this->acc_y, this->acc_z);
  this->roll = (int16_t)(filter.getRoll());
  this->pitch = (int16_t)(filter.getPitch());
  this->yaw = (int16_t)(filter.getYaw());
  this->acc_comp = sqrt(this->acc_x * this->acc_x + this->acc_y * this->acc_y + this->acc_z * this->acc_z);
}