#include "led.h"
#include "global.h"

LED::LED()
{
}

LED::~LED()
{
}

/**
 * @brief 初期化関数
 *
 */
void LED::initialize()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * @brief 赤色点滅関数
 *
 * @param interval_on
 * @param interval_off
 */
void LED::redBlink(uint16_t interval_on, uint16_t interval_off)
{
  this->time_e = millis();
  if (this->isRedOn && this->time_e - this->time_s > interval_on)
  {
    // 点灯している場合
    // 点灯間隔分を超えたらオフ
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, HIGH);
    isRedOn = !isRedOn;
    this->time_s = this->time_e;
  }
  else if (!this->isRedOn && this->time_e - this->time_s > interval_off)
  {
    // 消灯している場合
    // 消灯間隔分を超えたらオン
    digitalWrite(LED_RED, LOW);
    isRedOn = !isRedOn;
    this->time_s = this->time_e;
  }
  else
  {
  }
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * @brief 緑色点滅関数
 *
 * @param interval_on
 * @param interval_off
 */
void LED::greenBlink(uint16_t interval_on, uint16_t interval_off)
{
  this->time_e = millis();
  if (this->isGreenOn && this->time_e - this->time_s > interval_on)
  {
    // 点灯している場合
    // 点灯間隔分を超えたらオフ
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, HIGH);
    isGreenOn = !isGreenOn;
    this->time_s = this->time_e;
  }
  else if (!this->isGreenOn && this->time_e - this->time_s > interval_off)
  {
    // 消灯している場合
    // 消灯間隔分を超えたらオン
    digitalWrite(LED_GREEN, LOW);
    isGreenOn = !isGreenOn;
    this->time_s = this->time_e;
  }
  else
  {
  }
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * @brief 青色点滅関数
 *
 * @param interval_on
 * @param interval_off
 */
void LED::blueBlink(uint16_t interval_on, uint16_t interval_off)
{
  this->time_e = millis();
  if (this->isBlueOn && this->time_e - this->time_s > interval_on)
  {
    // 点灯している場合
    // 点灯間隔分を超えたらオフ
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, HIGH);
    isBlueOn = !isBlueOn;
    this->time_s = this->time_e;
  }
  else if (!this->isBlueOn && this->time_e - this->time_s > interval_off)
  {
    // 消灯している場合
    // 消灯間隔分を超えたらオン
    digitalWrite(LED_BLUE, LOW);
    isBlueOn = !isBlueOn;
    this->time_s = this->time_e;
  }
  else
  {
  }
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
}

/**
 * @brief LEDのRGBの色を変える関数
 *
 * @param red
 * @param green
 * @param blue
 */
void LED::setLEDRGB(bool red, bool green, bool blue)
{
  if (!red)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);
  if (!green)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);
  if (!blue)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);
}