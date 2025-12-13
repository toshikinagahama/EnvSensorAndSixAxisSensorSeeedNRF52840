#include "MyTimer.h"
#include <Arduino.h>
#include <nrf52840.h>
#include "global.h"
#include <nrf52_bitfields.h>
#include "NRF52_MBED_TimerInterrupt.h"

NRF52_MBED_Timer ITimer1(NRF_TIMER_3);
NRF52_MBED_Timer ITimer2(NRF_TIMER_4);

bool timer1_flg = false;
bool timer2_flg = false;
bool timer3_flg = false;
uint16_t timer1_count = 0;

/**
 * @brief 初期化関数
 *
 */
void timer_initialize()
{
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1);
  ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS * 1000, TimerHandler2);
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

void TimerHandler1()
{
  timer1_flg = true;
  timer1_count++;
  if (timer1_count >= 10) // 1秒ごとにタイマー3のイベントを発生させる
  {
    timer1_count = 0;
    timer3_flg = true;
    // enqueue(EVT_TIMER1_TIMEOUT, NULL, 0); これだと、なぜかハングアップしてしまうので、違うやり方でやる。
  }
}

void TimerHandler2()
{
  timer2_flg = true;
  // enqueue(EVT_TIMER2_TIMEOUT, NULL, 0); これだと、なぜかハングアップしてしまうので、違うやり方でやる。
}
