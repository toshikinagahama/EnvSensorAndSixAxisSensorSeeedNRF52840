#include "MyTimer.h"
#include <Arduino.h>
#include <bluefruit.h>
#include "MyGlobal.h"

SoftwareTimer ITimer1;
SoftwareTimer ITimer2;

bool timer1_flg = false;
bool timer2_flg = false;
bool timer3_flg = false;
uint16_t timer1_count = 0;

void TimerHandler1(TimerHandle_t xTimerID);
void TimerHandler2(TimerHandle_t xTimerID);

/**
 * @brief 初期化関数
 *
 */
void timer_initialize()
{
  ITimer1.begin(TIMER1_INTERVAL_MS, TimerHandler1);
  ITimer2.begin(TIMER2_INTERVAL_MS, TimerHandler2);
  ITimer1.start();
  ITimer2.start();
}

/**
 * @brief タイマーの更新関数
 *
 */
void timer_update()
{
  // アップデート
  if (timer1_flg == true)
  {
    timer1_flg = false;
    enqueue(EVT_TIMER1_TIMEOUT, NULL, 0);
  }

  if (timer2_flg == true)
  {
    timer2_flg = false;
    enqueue(EVT_TIMER2_TIMEOUT, NULL, 0);
  }

  if (timer3_flg == true)
  {
    timer3_flg = false;
    enqueue(EVT_TIMER3_TIMEOUT, NULL, 0);
  }
}

void TimerHandler1(TimerHandle_t xTimerID)
{
  timer1_flg = true;
  timer1_count++;
  if (timer1_count >= 10) // 1秒ごとにタイマー3のイベントを発生させる
  {
    timer1_count = 0;
    timer3_flg = true;
  }
}

void TimerHandler2(TimerHandle_t xTimerID)
{
  timer2_flg = true;
}
