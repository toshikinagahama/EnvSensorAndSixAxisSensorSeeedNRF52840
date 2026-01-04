#ifndef MYBATTERYSENSOR_H
#define MYBATTERYSENSOR_H
#include <Arduino.h>

class MyBatterySensor {

private:
  const uint8_t PIN_WAKEUP = P0_14;
  const uint8_t PIN_READ = PIN_VBAT;
  const uint8_t PIN_CHARGE_HIGH = P0_13;

public:
  float raw_vol = 0.0;
  uint8_t level = 0;
  uint8_t levels[1000] = {0}; // バッテリーレベルの履歴
  uint16_t cnt = 0;
  void initialize();
  uint8_t getValue();
};

#endif // MYBATTERYSENSOR_H