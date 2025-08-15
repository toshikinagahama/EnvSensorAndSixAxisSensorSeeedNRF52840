#include "MyTimer.h"
#include <Arduino.h>
#include <nrf52840.h>
#include "global.h"
#include <nrf52_bitfields.h>
#include "NRF52_MBED_TimerInterrupt.h"

NRF52_MBED_Timer ITimer0(NRF_TIMER_3);
NRF52_MBED_Timer ITimer1(NRF_TIMER_4);

bool timer1_flg = false;
bool timer2_flg = false;
/**
 * @brief 初期化関数
 *
 */
void timer_initialize()
{
  ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0);
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1);
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
}

void TimerHandler0()
{
  timer1_flg = true;
  // enqueue(EVT_TIMER1_TIMEOUT, NULL, 0); これだと、なぜかハングアップしてしまうので、違うやり方でやる。
}

void TimerHandler1()
{
  timer2_flg = true;
  // enqueue(EVT_TIMER2_TIMEOUT, NULL, 0); これだと、なぜかハングアップしてしまうので、違うやり方でやる。
}