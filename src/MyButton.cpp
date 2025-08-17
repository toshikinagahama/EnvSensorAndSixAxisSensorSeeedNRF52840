#include "MyButton.h"
#include <Arduino.h>
#include <nrf52840.h>
#include <nrfx_gpiote.h>
#include "global.h"

unsigned long buttonA_press_time = 0;
uint8_t buttonA_press_count = 0; // ボタンAの押下回数
bool buttonA_pressed = false;
bool buttonA_long1_press_detected = false;
bool buttonA_long2_press_detected = false;
bool buttonA_short_press_detected = false;

void gpio_wakeup_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  // このハンドラは復帰時に呼び出されるが、ここでは何もしない
  // System OFFから復帰するとsetup()が最初から実行されるため
}

// 復帰用のGPIOピンを設定する関数
void setup_wakeup_gpio()
{
  if (!nrfx_gpiote_is_init())
  {
    nrfx_gpiote_init();
  }

  // HIGHからLOWへの変化（ボタン押下）で割り込みを発生
  nrfx_gpiote_in_config_t in_config;
  in_config.pull = NRF_GPIO_PIN_PULLUP;
  in_config.is_watcher = false; // ウォッチャーモードは使用しない
  in_config.sense = NRF_GPIOTE_POLARITY_HITOLO;

  nrfx_gpiote_in_init(digitalPinToPinName(PIN_BUTTON_A), &in_config, gpio_wakeup_handler);
  nrfx_gpiote_in_event_enable(digitalPinToPinName(PIN_BUTTON_A), true);
}

/**
 * @brief 初期化関数
 *
 */
void button_initialize()
{
  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  setup_wakeup_gpio(); // 復帰用のGPIOピンを設定
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_A), buttonA_ISR, CHANGE);
}

/**
 * @brief ボタンの更新関数
 *
 */
void button_update()
{
  // アップデート
  if (buttonA_short_press_detected == true)
  {
    buttonA_short_press_detected = false;
    enqueue(EVT_BUTTON_A_SHORT_PRESSED, NULL, 0);
  }

  if (buttonA_long1_press_detected == true)
  {
    buttonA_long1_press_detected = false;
    enqueue(EVT_BUTTON_A_LONG1_PRESSED, NULL, 0);
  }

  if (buttonA_long2_press_detected == true)
  {
    buttonA_long2_press_detected = false;
    enqueue(EVT_BUTTON_A_LONG2_PRESSED, NULL, 0);
  }
}

/**
 * @brief ボタンA割り込み関数
 *
 */
void buttonA_ISR()
{
  if (digitalRead(PIN_BUTTON_A) == LOW)
  {
    // ボタンが押された瞬間 (LOW)
    buttonA_press_time = millis();
    buttonA_pressed = true;
  }
  else
  {
    // ボタンが離された瞬間 (HIGH)
    if (buttonA_pressed)
    {
      unsigned long duration = millis() - buttonA_press_time;
      if (duration >= LONG1_PRESS_THRESHOLD_MS)
      {
        if (duration >= LONG2_PRESS_THRESHOLD_MS)
        {
          buttonA_long2_press_detected = true; // 長押し2のイベント
        }
        else
        {
          buttonA_long1_press_detected = true; // 長押し1のイベント
        }
      }
      else
      {
        buttonA_short_press_detected = true;
      }
      buttonA_pressed = false; // フラグをリセット
    }
  }
}