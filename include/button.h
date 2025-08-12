#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#define PIN_BUTTON_A P1_13
#define LONG_PRESS_THRESHOLD_MS 10000

extern void button_initialize();
extern void buttonA_ISR();
#endif