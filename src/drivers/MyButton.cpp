#include "MyButton.h"
#include <Arduino.h>
#include <nrf52840.h>
#include <nrfx_gpiote.h>
#include "MyGlobal.h"

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
  nrf_gpio_cfg_sense_input(digitalPinToPinName(PIN_BUTTON_A), NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
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

void button_update()
{
  // No-op: Events are enqueued directly in the ISR to support sleep mode.
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
          enqueue(EVT_BUTTON_A_LONG2_PRESSED, NULL, 0); // 長押し2のイベント
        }
        else
        {
          enqueue(EVT_BUTTON_A_LONG1_PRESSED, NULL, 0); // 長押し1のイベント
        }
      }
      else
      {
        enqueue(EVT_BUTTON_A_SHORT_PRESSED, NULL, 0);
      }
      buttonA_pressed = false; // フラグをリセット
    }
  }
}