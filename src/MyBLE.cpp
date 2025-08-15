#include "MyBLE.h"
#include "global.h"

MyBLE::MyBLE()
{
  // コンストラクタ
  this->SENSOR_Service = new BLEService(BLE_SENSOR_SERVICE_UUID);
  this->Battery_Service = new BLEService(BLE_BATTERY_SERVICE_UUID);
  this->SENSOR_TX_Chara = new BLECharacteristic(BLE_SENSOR_TX_CHARA_UUID, BLENotify, 25);
  this->SENSOR_RX_Chara = new BLECharacteristic(BLE_SENSOR_RX_CHARA_UUID, BLEWrite, 7);
  this->Battery_chara = new BLEIntCharacteristic(BLE_BATTERY_CHARA_UUID, BLERead | BLENotify);
  this->SENSOR_Descriptor = new BLEDescriptor(BLE_DESCRIPTOR_UUID, "SENSOR");
  this->Battery_Descriptor = new BLEDescriptor(BLE_DESCRIPTOR_UUID, "Battery Level: 0 - 100");
}

MyBLE::~MyBLE()
{
  // デストラクタ
  delete this->SENSOR_Service;
  delete this->Battery_Service;
  delete this->SENSOR_TX_Chara;
  delete this->SENSOR_RX_Chara;
  delete this->Battery_chara;
  delete this->Battery_Descriptor;
  delete this->SENSOR_Descriptor;
}

void MyBLE::BatteryCharaReadHandler(BLEDevice central, BLECharacteristic chara)
{
  // バッテリーチャラクタの読み取りハンドラ
  uint8_t batteryLevel = batterySensor->getValue(); // 一旦。本当はイベントキューに入れないと。あんまりやりたくないけど、ここで直接取得
  chara.writeValue(batteryLevel);
}

void MyBLE::SensorCharaWrittenHandler(BLEDevice central, BLECharacteristic chara)
{
  // 受け取ったメッセージごとに処理
  // Serial.println(chara.value());
  uint8_t data[2];
  chara.readValue(data, 2);

  // debug
  Serial.print(data[0]);
  Serial.print(",");
  Serial.println(data[1]);
  switch (data[0])
  {
  case 0x00:
    // 機器情報関連
    switch (data[1])
    {
    case 0x01:
      // 機器情報取得
      enqueue(EVT_BLE_CMD_GET_DEVICE_INFO, NULL, 0);
      break;
    case 0x02:
      // タイムスタンプ取得
      enqueue(EVT_BLE_CMD_GET_START_TIMESTAMP, NULL, 0);
      break;
    case 0x03:
      // タイムスタンプ設定
      {
        uint8_t tmp[6];
        chara.readValue(tmp, 6); // 6バイトの値を読み込む
        enqueue(EVT_BLE_CMD_SET_START_TIMESTAMP, &tmp[2], 6);
      }
      break;
    case 0x04:
      // 現在のタイムスタンプ取得
      enqueue(EVT_BLE_CMD_GET_TIMESTAMP, NULL, 0);
      break;
    case 0x05:
      // 現在のデータページNO取得
      enqueue(EVT_BLE_CMD_GET_DATA_PAGE_NO, NULL, 0);
      break;
    default:
      break;
    }
    break;
  case 0x01:
    // 測定関係
    switch (data[1])
    {
    case 0x00:
      // 測定開始
      enqueue(EVT_BLE_CMD_MEAS_START, NULL, 0);
      break;
    case 0x01:
      // 測定終了
      enqueue(EVT_BLE_CMD_MEAS_STOP, NULL, 0);
      break;
    default:
      break;
    }
    break;
  case 0x02:
    // データ読み出し関係
    switch (data[1])
    {
    case 0x00:
      // 1バイトデータ取得
      {
        uint8_t tmp[5];
        chara.readValue(tmp, 5); // 7バイトの値を読み込む（3byte目はページ番号、4~5byte目はデータ番号）
        enqueue(EVT_BLE_CMD_GET_DATA_1_DATA, &tmp[2], 3);
      }
      break;
    case 0x01:
      // 最新のデータ取得
      {
        enqueue(EVT_BLE_CMD_GET_LATEST_DATA, NULL, 0);
      }
      break;
    default:
      break;
    }
    break;
  default:
    Serial.println("Invalid cmd");
    break;
  }
}

void MyBLE::SensorCharaReadHandler(BLEDevice central, BLECharacteristic chara)
{
}

void MyBLE::blePeripheralConnectHandler(BLEDevice central)
{

  Serial.print("Connected event, central: ");
  Serial.println(central.address());
  enqueue(EVT_BLE_CONNECTED, NULL, 0);
}

void MyBLE::blePeripheralDisconnectHandler(BLEDevice central)
{
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
  enqueue(EVT_BLE_DISCONNECTED, NULL, 0);
}

void MyBLE::initialize()
{
  BLE.end();
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1)
      ;
  }
  BLE.setConnectable(true); // 接続可能にする
  BLE.setPairable(true);    // ペアリング可能にする
  BLE.setEventHandler(BLEConnected, this->blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, this->blePeripheralDisconnectHandler);
  BLE.setDeviceName(BLE_LOCAL_NAME);
  BLE.setLocalName(BLE_LOCAL_NAME);
  BLE.setAdvertisedService(*this->SENSOR_Service);                 // add the service UUID
  BLE.setAdvertisedService(*this->Battery_Service);                // add the service UUID
  this->Battery_chara->addDescriptor(*this->Battery_Descriptor);   // add descriptor
  this->SENSOR_Service->addCharacteristic(*this->SENSOR_TX_Chara); // add characteristic
  this->SENSOR_Service->addCharacteristic(*this->SENSOR_RX_Chara); // add characteristic
  this->Battery_Service->addCharacteristic(*this->Battery_chara);  // add characteristic
  this->SENSOR_TX_Chara->addDescriptor(*this->SENSOR_Descriptor);  // add descriptor
  this->SENSOR_RX_Chara->addDescriptor(*this->SENSOR_Descriptor);  // add descriptor
  this->Battery_chara->setEventHandler(BLERead, this->BatteryCharaReadHandler);
  this->SENSOR_TX_Chara->setEventHandler(BLERead, this->SensorCharaReadHandler);
  this->SENSOR_RX_Chara->setEventHandler(BLEWritten, this->SensorCharaWrittenHandler);
  BLE.addService(*this->Battery_Service);
  BLE.addService(*this->SENSOR_Service);
}

void MyBLE::poll()
{
  BLE.poll();
}

void MyBLE::advertiseStart()
{
  BLE.advertise();
}

void MyBLE::advertiseStop()
{
  BLE.stopAdvertise();
}
