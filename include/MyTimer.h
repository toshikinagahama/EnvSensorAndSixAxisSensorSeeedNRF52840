#ifndef MYTIMER_H
#define MYTIMER_H
#include <Arduino.h>

#define TIMER0_INTERVAL_MS 1000
#define TIMER1_INTERVAL_MS 5000

void TimerHandler0();
void TimerHandler1();

// 外部定義
extern void timer_initialize();
extern void timer_update();
#endif