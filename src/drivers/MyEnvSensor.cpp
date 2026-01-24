#include "MyEnvSensor.h"
#include "global.h"

MyEnvSensor::MyEnvSensor() {}

MyEnvSensor::~MyEnvSensor() {}

void MyEnvSensor::initialize()
{
  Wire.begin();

  Wire.beginTransmission(MLX90614_ADDRESS);
  Wire.write(0xFF);
  Wire.write(0xE8);
  Wire.endTransmission();

  // 2. SCLラインをLowに落としてスリープを確定させる
  // Wireライブラリを一度終了し、ピン操作を行う
  Wire.end();
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
}

void MyEnvSensor::getValue()
{
  float temp;
  if (GetTempFromMLX90614(MLX90614_ADDRESS, 'O', &temp))
  {
    this->tmp_obj = temp;
  }
  if (GetTempFromMLX90614(MLX90614_ADDRESS, 'A', &temp))
  {
    this->tmp_env = temp;
  }
}

// http://blog.livedoor.jp/ktkt_91st/archives/1020073547.htmlから
/* Function that recieves few bytes from resister of MLX90614 *
 *    arguments                                                *
 *     addr   : slave address                                  *
 *     cmd    : resister address                               *
 *     *array : address of array to contain data               *
 *     return : true if success, false if failed               *
 **************************************************************/
bool MyEnvSensor::ReadFromMLX90614(char addr, char cmd, char *arry)
{
  char i;

  Wire.beginTransmission(addr); // start recieve process
  Wire.write(cmd);              // Write command
  Wire.endTransmission(false);  // publish repeat start condition

  Wire.requestFrom(addr, 3); // request data+pec byte

  // No retry/blocking loop. If data isn't ready immediately, treat as fail.
  // The user prioritizes non-blocking over guaranteed read.
  if (Wire.available() < 3)
  {
    return false;
  }

  for (i = 0; i < 2; i++)
  {
    arry[i] = Wire.read(); // read data
  }
  Wire.read();            // pec byte
  Wire.endTransmission(); // end recieve process
  return true;
}

/* Function that reads temperature from MLX90614 *
 *      arguments                                 *
 *        addr : slave address                    *
 *        type : type of temperature              *
 *                  'O' or 'o' : reads object     *
 *                  other      : reads ambient    *
 *        *value : pointer to store result        *
 *      return                                    *
 *        true if success                         *
 *************************************************/
bool MyEnvSensor::GetTempFromMLX90614(char addr, char type, float *value)
{
  int tmp;
  char cmd;
  char dat[2];
  float temp;

  cmd = (type == 'O' || type == 'o') ? (0x07) : (0x06); // determin command
  if (!ReadFromMLX90614(addr, cmd, dat))
  {
    return false; // Read failed
  }

  tmp = (dat[1] << 8) + dat[0]; // to temperature in marge two bytes

  // Check for Invalid Zero Reading (Communication Error)
  if (tmp == 0)
  {
    return false;
  }

  temp =
      (((float)tmp * 2.0) - 27315.0) /
      100.0; // convert Absolute temperature to Degrees Celsius of temperature

  *value = temp;
  return true;
}