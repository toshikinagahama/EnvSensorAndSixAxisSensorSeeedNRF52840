#ifndef MYLED_H
#define MYLED_H
// インクルード
#include <Arduino.h>

enum Color
{
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_MAX,
};

class MyLed
{

private:
  // メンバ
  ulong time_s = 0; // タイマー用
  ulong time_e = 0; // タイマー用
  bool isRedOn = false;
  bool isGreenOn = false;
  bool isBlueOn = false;

public:
  //  関数
  MyLed();
  ~MyLed();
  void initialize();
  void blink(Color, uint16_t, uint16_t);
  void redBlink(uint16_t, uint16_t);
  void greenBlink(uint16_t, uint16_t);
  void blueBlink(uint16_t, uint16_t);
  void setLEDRGB(bool, bool, bool);
};
#endif