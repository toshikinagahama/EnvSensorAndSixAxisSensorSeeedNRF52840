#include "MyLed.h"

MyLed::MyLed() {}

MyLed::~MyLed() {}

// 255 = OFF (Active Low), 0 = MAX Brightness
// 250 is very dim (low duty cycle)
#define LED_OFF 255
#define LED_ON_DIM 250

/**
 * @brief 初期化関数
 *
 */
void MyLed::initialize() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  // Ensure all off
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * @brief 点滅関数
 *
 * @param color
 * @param interval_on
 * @param interval_off
 */
void MyLed::blink(Color color, uint16_t interval_on, uint16_t interval_off) {
  this->time_e = millis();

  switch (color) {
  case COLOR_RED:
    if (this->isRedOn && this->time_e - this->time_s > interval_on) {
      // 点灯している場合
      // 点灯間隔分を超えたらオフ
      setLEDRGB(false, false, false);
      isRedOn = !isRedOn;
      this->time_s = this->time_e;
    } else if (!this->isRedOn && this->time_e - this->time_s > interval_off) {
      // 消灯している場合
      // 消灯間隔分を超えたらオン
      setLEDRGB(false, false, false);
      digitalWrite(LED_RED, LOW);
      isRedOn = !isRedOn;
      this->time_s = this->time_e;
    }
    break;
  case COLOR_GREEN:
    if (this->isGreenOn && this->time_e - this->time_s > interval_on) {
      // 点灯している場合
      // 点灯間隔分を超えたらオフ
      setLEDRGB(false, false, false);
      isGreenOn = !isGreenOn;
      this->time_s = this->time_e;
    } else if (!this->isGreenOn && this->time_e - this->time_s > interval_off) {
      // 消灯している場合
      // 消灯間隔分を超えたらオン
      setLEDRGB(false, false, false);
      digitalWrite(LED_GREEN, LOW);
      isGreenOn = !isGreenOn;
      this->time_s = this->time_e;
    }
    break;
  case COLOR_BLUE:
    if (this->isBlueOn && this->time_e - this->time_s > interval_on) {
      // 点灯している場合
      // 点灯間隔分を超えたらオフ
      setLEDRGB(false, false, false);
      isBlueOn = !isBlueOn;
      this->time_s = this->time_e;
    } else if (!this->isBlueOn && this->time_e - this->time_s > interval_off) {
      // 消灯している場合
      // 消灯間隔分を超えたらオン
      setLEDRGB(false, false, false);
      digitalWrite(LED_BLUE, LOW);
      isBlueOn = !isBlueOn;
      this->time_s = this->time_e;
    }
    break;
  default:
    // COLOR_MAXの場合は何もしない
    break;
  }
}

/**
 * @brief 赤色点滅関数
 *
 * @param interval_on
 * @param interval_off
 */
void MyLed::redBlink(uint16_t interval_on, uint16_t interval_off) {
  this->time_e = millis();
  if (this->isRedOn && this->time_e - this->time_s > interval_on) {
    // 点灯している場合 -> オフにする
    digitalWrite(LED_RED, HIGH);
    isRedOn = !isRedOn;
    this->time_s = this->time_e;
  } else if (!this->isRedOn && this->time_e - this->time_s > interval_off) {
    // 消灯している場合 -> オンにする (PWMで暗く)
    analogWrite(LED_RED, LED_ON_DIM);
    isRedOn = !isRedOn;
    this->time_s = this->time_e;
  }

  // 他の色は確実にOFF
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * @brief 緑色点滅関数
 *
 * @param interval_on
 * @param interval_off
 */
void MyLed::greenBlink(uint16_t interval_on, uint16_t interval_off) {
  this->time_e = millis();
  if (this->isGreenOn && this->time_e - this->time_s > interval_on) {
    // 点灯している場合 -> オフ
    digitalWrite(LED_GREEN, HIGH);
    isGreenOn = !isGreenOn;
    this->time_s = this->time_e;
  } else if (!this->isGreenOn && this->time_e - this->time_s > interval_off) {
    // 消灯している場合 -> オン (PWM)
    analogWrite(LED_GREEN, LED_ON_DIM);
    isGreenOn = !isGreenOn;
    this->time_s = this->time_e;
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
void MyLed::blueBlink(uint16_t interval_on, uint16_t interval_off) {
  this->time_e = millis();
  if (this->isBlueOn && this->time_e - this->time_s > interval_on) {
    // 点灯 -> オフ
    digitalWrite(LED_BLUE, HIGH);
    isBlueOn = !isBlueOn;
    this->time_s = this->time_e;
  } else if (!this->isBlueOn && this->time_e - this->time_s > interval_off) {
    // 消灯 -> オン (PWM)
    analogWrite(LED_BLUE, LED_ON_DIM);
    isBlueOn = !isBlueOn;
    this->time_s = this->time_e;
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
void MyLed::setLEDRGB(bool red, bool green, bool blue) {
  if (!red)
    digitalWrite(LED_RED, HIGH);
  else
    analogWrite(LED_RED, LED_ON_DIM);

  if (!green)
    digitalWrite(LED_GREEN, HIGH);
  else
    analogWrite(LED_GREEN, LED_ON_DIM);

  if (!blue)
    digitalWrite(LED_BLUE, HIGH);
  else
    analogWrite(LED_BLUE, LED_ON_DIM);
}