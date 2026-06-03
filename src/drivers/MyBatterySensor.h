#ifndef MYBATTERYSENSOR_H
#define MYBATTERYSENSOR_H
#include <Arduino.h>

class MyBatterySensor
{

private:
  const uint8_t PIN_WAKEUP = 14;      // VBAT_ENABLE (P0.14)
  const uint8_t PIN_READ = PIN_VBAT;  // PIN_VBAT (32, P0.31)
  const uint8_t PIN_CHARGE_HIGH = 22; // PIN_CHARGING_CURRENT (P0.13)

public:
  float raw_vol = 0.0;
  uint8_t level = 0;
  uint8_t levels[100] = {0}; // バッテリーレベルの履歴
  uint16_t cnt = 0;
  void initialize();
  uint8_t getValue();
};

#endif // MYBATTERYSENSOR_H