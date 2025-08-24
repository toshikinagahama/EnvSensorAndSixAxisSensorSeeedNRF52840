#ifndef MYDISPLAY_H
#define MYDISPLAY_H
// インクルード
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

class MyDisplay
{

private:
  // メンバ
  ulong time_s = 0; // タイマー用
  ulong time_e = 0; // タイマー用
  Adafruit_SSD1306 *display;

public:
  //  関数
  MyDisplay();
  ~MyDisplay();
  void initialize();
  void update();
};
#endif