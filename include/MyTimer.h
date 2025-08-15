#ifndef MYTIMER_H
#define MYTIMER_H
#include <Arduino.h>

#define TIMER1_INTERVAL_MS 100
#define TIMER2_INTERVAL_MS 5000

void TimerHandler1();
void TimerHandler2();

// 外部定義
extern void timer_initialize();
extern void timer_update();
#endif