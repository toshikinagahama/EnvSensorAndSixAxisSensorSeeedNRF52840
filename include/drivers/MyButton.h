#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#define PIN_BUTTON_A P1_13
#define LONG1_PRESS_THRESHOLD_MS 3000  // 長押し1の閾値
#define LONG2_PRESS_THRESHOLD_MS 10000 // 長押し2の閾値（deepsleepモード用）

extern void button_initialize();
extern void buttonA_ISR();
extern void button_update();
#endif