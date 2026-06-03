#include "MyBatterySensor.h"
#include <nrf52840.h>
#include <nrfx_saadc.h>

void MyBatterySensor::initialize()
{
  analogReference(AR_INTERNAL); // Use internal 0.6V ref with 1/6 gain (0 to 3.6V range)
  analogReadResolution(10);     // 10-bit A/D resolution

  pinMode(this->PIN_WAKEUP, OUTPUT);
  digitalWrite(this->PIN_WAKEUP, HIGH); // Keep divider disabled to save power

  // Charge Current Setting (High: 50mA, Low: 100mA)
  pinMode(this->PIN_CHARGE_HIGH, OUTPUT);
  digitalWrite(this->PIN_CHARGE_HIGH, LOW);

  // Initialize levels array with a first reading
  for (uint16_t i = 0; i < 100; i++)
  {
    // 100回回して安定をとる
    getValue();
    delay(5);
  }
}

uint8_t MyBatterySensor::getValue()
{
  const int max_voltage_mv = 4200; // Fully charged LiPo (4.2V)
  const int min_voltage_mv = 3100; // Empty LiPo (3.1V)

  // 1. Enable the voltage divider
  digitalWrite(this->PIN_WAKEUP, LOW);
  delay(2); // Wait for the analog voltage to stabilize

  // 2. Read the analog value
  int vbat_raw = analogRead(PIN_VBAT);

  // 3. Disable the divider immediately to save power
  digitalWrite(this->PIN_WAKEUP, HIGH);

  // 4. Calculate actual battery voltage (mV)
  // ADC VREF is 3.6V (3600mV) under AR_INTERNAL.
  // Voltage divider: R1 = 1M Ohm, R2 = 510k Ohm -> Vbat = Vpin * (1000 + 510) / 510
  uint32_t vpin_mv = ((uint32_t)vbat_raw * 3600) / 1023;
  uint16_t volt = (uint16_t)((vpin_mv * 1510) / 510);
  // Serial.print("Battery Voltage (mV): ");
  // Serial.println(volt);

  // Convert voltage to percentage (0% to 100%)
  int battery_percent = (int)((float)(volt - min_voltage_mv) / (float)(max_voltage_mv - min_voltage_mv) * 100.0f);
  if (battery_percent > 100)
    battery_percent = 100;
  if (battery_percent < 1)
    battery_percent = 1;

  levels[cnt] = battery_percent; // Save to history
  cnt++;
  if (cnt >= 100)
    cnt = 0; // Reset history counter

  // Calculate moving average over history
  float sum = 0.0;
  for (uint16_t i = 0; i < 100; i++)
  {
    sum += levels[i];
  }
  return (uint8_t)(sum / 100.0f);
}