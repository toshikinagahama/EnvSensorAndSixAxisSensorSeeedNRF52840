#include "MySensor.h"
#include "MyGlobal.h"

MySensor::MySensor()
{
  this->IMU = new LSM6DS3(I2C_MODE, 0x6A);
}

MySensor::~MySensor()
{
}

void MySensor::initialize()
{
  // Onboard IMU power control (needs to be enabled on Adafruit core)
  pinMode(PIN_LSM6DS3TR_C_POWER, OUTPUT);
  digitalWrite(PIN_LSM6DS3TR_C_POWER, HIGH);
  delay(50); // Wait for sensor power to stabilize

  filter.begin(10);                    // 10Hzで初期化
  this->IMU->settings.gyroRange = 500; // そんなに激しい動きはしないので、500dpsで十分
  this->IMU->settings.accelRange = 4;  // そんなに激しい動きはしないので、4gで十分
  while (this->IMU->begin() != 0)
  {
    Serial.println("Failed to initialize IMU!");
  }
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, 0x8C);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x4A);
  this->IMU->writeRegister(0x17, 0x04);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL7_G, 0x00);
  // this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL8_XL, 0x09);
}

void MySensor::sleep()
{
  // 加速度センサーをパワーダウン (CTRL1_XL レジスタに 0x00 を書き込む)
  this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x00);

  // ジャイロセンサーをパワーダウン (CTRL2_G レジスタに 0x00 を書き込む)
  this->IMU->writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, 0x00);
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