#ifndef MYBLE_H
#define MYBLE_H
// インクルード
#include <Arduino.h>
#include <bluefruit.h>

// マクロ定義
#define BLE_SENSOR_SERVICE_UUID "4fafc202-1fb5-459e-8fcc-c5c9c331914b"  // BLE Service
#define BLE_BATTERY_SERVICE_UUID "0000180f-0000-1000-8000-00805f9b34fb" // BLE Service

#define BLE_SENSOR_RX_CHARA_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // BLE RX(スマホ→デバイス) Characteristic
#define BLE_SENSOR_TX_CHARA_UUID "beb5483f-36e1-4688-b7f5-ea07361b26a8" // BLE RX(デバイス→スマホ)  Characteristic
#define BLE_BATTERY_CHARA_UUID "00002a19-0000-1000-8000-00805f9b34fb"

#define BLE_LOCAL_NAME "HAMA_TEMP_SENSOR"

/**
 * @brief BLECharacteristic互換ラッパークラス
 */
class MyBLECharacteristic : public BLECharacteristic
{
public:
  MyBLECharacteristic(const char* uuid) : BLECharacteristic(uuid) {}
  MyBLECharacteristic(uint16_t uuid) : BLECharacteristic(uuid) {}

  bool writeValue(const void* data, uint16_t len) {
    write(data, len);
    Serial.print("Properties notify value: ");
    Serial.println(this->_properties.notify);
    if (this->_properties.notify) {
      bool res = notify(data, len);
      Serial.print("BLE Notify Status: ");
      Serial.println(res);
      return res;
    }
    return true;
  }

  bool writeValue(uint8_t val) {
    return writeValue(&val, 1);
  }

  bool writeValue(uint32_t val) {
    return writeValue(&val, sizeof(val));
  }

  uint16_t readValue(void* buffer, uint16_t len) {
    return read(buffer, len);
  }
};

/**
 *
 * BLEクラス
 *
 */
class MyBLE
{
public:
  // メンバ
  BLEService *SENSOR_Service;
  BLEService *Battery_Service;
  MyBLECharacteristic *SENSOR_RX_Chara;
  MyBLECharacteristic *SENSOR_TX_Chara;
  MyBLECharacteristic *Battery_chara;
  bool isConnect = false;

  //  関数
  MyBLE();
  ~MyBLE();
  void initialize();
  void poll();
  void advertiseStart();
  void advertiseStop();
};

#endif // MYBLE_H