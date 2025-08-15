#ifndef MYBLE_H
#define MYBLE_H
// インクルード
#include <Arduino.h>
#include <ArduinoBLE.h>
#include <nrf52840.h>

// マクロ定義
#define BLE_SENSOR_SERVICE_UUID "4fafc202-1fb5-459e-8fcc-c5c9c331914b"  // BLE Service
#define BLE_BATTERY_SERVICE_UUID "0000180f-0000-1000-8000-00805f9b34fb" // BLE Service

#define BLE_SENSOR_RX_CHARA_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // BLE RX(スマホ→デバイス) Characteristic
#define BLE_SENSOR_TX_CHARA_UUID "beb5483f-36e1-4688-b7f5-ea07361b26a8" // BLE RX(デバイス→スマホ)  Characteristic
#define BLE_BATTERY_CHARA_UUID "00002a19-0000-1000-8000-00805f9b34fb"

#define BLE_DESCRIPTOR_UUID "2901" // BLE  Descriptor

#define BLE_LOCAL_NAME "HAMA_TEMP_SENSOR"

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
  BLECharacteristic *SENSOR_RX_Chara;
  BLECharacteristic *SENSOR_TX_Chara;
  BLEIntCharacteristic *Battery_chara;
  BLEDescriptor *Battery_Descriptor;
  BLEDescriptor *SENSOR_Descriptor;
  bool isConnect = false;
  //  関数
  MyBLE();
  ~MyBLE();
  void initialize();
  void poll();
  void advertiseStart();
  void advertiseStop();
  static void BatteryCharaReadHandler(BLEDevice central, BLECharacteristic chara);
  static void SensorCharaReadHandler(BLEDevice central, BLECharacteristic chara);
  static void SensorCharaWrittenHandler(BLEDevice central, BLECharacteristic chara);
  static void blePeripheralConnectHandler(BLEDevice central);
  static void blePeripheralDisconnectHandler(BLEDevice central);
};

#endif // MYBLE_H