#ifndef LED_H
#define LED_H
// インクルード
#include <Arduino.h>
#include <nrf52840.h>
#include "global.h"

class LED
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
  LED();
  ~LED();
  void initialize();
  void redBlink(uint16_t, uint16_t);
  void greenBlink(uint16_t, uint16_t);
  void blueBlink(uint16_t, uint16_t);
  void setLEDRGB(bool, bool, bool);
};
#endif