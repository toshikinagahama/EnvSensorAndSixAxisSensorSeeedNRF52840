#include "MyBatterySensor.h"
#include <nrf52840.h>
#include <nrfx_saadc.h>

void MyBatterySensor::initialize()
{
  analogReference(0);       // 0はAR_VDDで3.3V
  analogReadResolution(10); // 10bit A/D
  pinMode(this->PIN_WAKEUP, OUTPUT);
  digitalWrite(this->PIN_WAKEUP, LOW);
  // pinMode(this->PIN_READ, INPUT);
}

uint8_t MyBatterySensor::getValue()
{

  const int max_voltage_mv = 3894; //
  const int min_voltage_mv = 3000; //

  // バッテリー電圧の測定
  int vbat_raw = analogRead(PIN_VBAT);
  // int vbat_raw = 1;
  int vbat_mv = vbat_raw * 3300 / 1023; // VREF = 2.4V, 10bit A/D
  // vbat_mv = vbat_mv * 1510 / 510;       // 1M + 510k / 510k
  uint16_t volt = (uint16_t)(vbat_mv * 1.18);
  // int battery_percent = map(vbat_mv, min_voltage_mv, max_voltage_mv, 0, 100);
  int battery_percent = (int)((float)(volt - min_voltage_mv) / (float)(max_voltage_mv - min_voltage_mv) * 100.0f);
  if (battery_percent > 100)
    battery_percent = 100;
  if (battery_percent < 1)
    battery_percent = 1;
  // Serial.println(volt);
  return battery_percent;
  // return battery_percent;
}