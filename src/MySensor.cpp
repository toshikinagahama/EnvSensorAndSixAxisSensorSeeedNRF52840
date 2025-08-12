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
  this->IMU->settings.gyroRange = 2000;
  this->IMU->settings.accelRange = 16;
  while (this->IMU->begin() != 0)
  {
  }
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, 0x8C);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x8A);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL7_G, 0x00);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL8_XL, 0x09);
}

void MySensor::getValue()
{
  this->acc_x = this->IMU->readRawAccelX();
  this->acc_y = this->IMU->readRawAccelY();
  this->acc_z = this->IMU->readRawAccelZ();
  this->gyr_x = this->IMU->readRawGyroX();
  this->gyr_y = this->IMU->readRawGyroY();
  this->gyr_z = this->IMU->readRawGyroZ();
}