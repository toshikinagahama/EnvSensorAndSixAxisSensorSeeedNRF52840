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
}

void MyEnvSensor::getValue()
{

  this->tmp_obj = ReadTempFromMLX90614(MLX90614_ADDRESS, 'O');
  this->tmp_env = ReadTempFromMLX90614(MLX90614_ADDRESS, 'A');
}

// http://blog.livedoor.jp/ktkt_91st/archives/1020073547.htmlから
/* Function that recieves few bytes from resister of MLX90614 *
 *    arguments                                                *
 *     addr   : slave address                                  *
 *     cmd    : resister address                               *
 *     *array : address of array to contain data               *
 **************************************************************/
void MyEnvSensor::ReadFromMLX90614(char addr, char cmd, char *arry)
{
  char i;

  Wire.beginTransmission(addr); // start recieve process
  Wire.write(cmd);              // Write command
  Wire.endTransmission(false);  // publish repeat start condition

  Wire.requestFrom(addr, 3); // request data+pec byte
  while (Wire.available() < 3)
  {
    // Wire.beginTransmission(addr); // start recieve process
    // Wire.write(cmd);              // Write command
    // Wire.endTransmission(false);  // publish repeat start condition

    Wire.requestFrom(addr, 3); // request data+pec byte
  }
  for (i = 0; i < 2; i++)
  {
    arry[i] = Wire.read(); // read data
  }
  Wire.read();            // pec byte
  Wire.endTransmission(); // end recieve process
}

/* Function that reads temperature from MLX90614 *
 *      arguments                                 *
 *        addr : slave address                    *
 *        type : type of temperature              *
 *                  'O' or 'o' : reads object     *
 *                  other      : reads ambient    *
 *      return                                    *
 *        Degrees Celsius of temperature          *
 *************************************************/
float MyEnvSensor::ReadTempFromMLX90614(char addr, char type)
{
  int tmp;
  char cmd;
  char dat[2];
  float temp;

  cmd = (type == 'O' || type == 'o') ? (0x07) : (0x06); // determin command
  ReadFromMLX90614(addr, cmd, dat);                     // recieve from MLX90614
  tmp = (dat[1] << 8) + dat[0];                         // to temperature in marge two bytes
  temp = (((float)tmp * 2.0) - 27315.0) / 100.0;        // convert Absolute temperature to Degrees Celsius of temperature

  return temp;
}