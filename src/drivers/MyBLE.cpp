#include "MyBLE.h"
#include "MyGlobal.h"

// Connection callback
void connect_callback(uint16_t conn_handle)
{
  BLEConnection *conn = Bluefruit.Connection(conn_handle);
  char peer_addr[18] = {0};
  if (conn)
  {
    ble_gap_addr_t addr = conn->getPeerAddr();
    sprintf(peer_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr.addr[5], addr.addr[4], addr.addr[3],
            addr.addr[2], addr.addr[1], addr.addr[0]);
  }
  Serial.print("Connected event, central: ");
  Serial.println(peer_addr);

  if (ble)
    ble->isConnect = true;
  enqueue(EVT_BLE_CONNECTED, NULL, 0);
}

// Disconnection callback
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  Serial.print("Disconnected event, reason: 0x");
  Serial.println(reason, HEX);
  if (ble)
    ble->isConnect = false;
  enqueue(EVT_BLE_DISCONNECTED, NULL, 0);
}

// Write callback for RX Characteristic
void SensorCharaWrittenCallback(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
{
  if (len < 2)
    return;
  Serial.print(data[0]);
  Serial.print(",");
  Serial.println(data[1]);

  switch (data[0])
  {
  case 0x00:
    switch (data[1])
    {
    case 0x01:
      enqueue(EVT_BLE_CMD_GET_DEVICE_INFO, NULL, 0);
      break;
    case 0x02:
      enqueue(EVT_BLE_CMD_GET_START_TIMESTAMP, NULL, 0);
      break;
    case 0x03:
      if (len >= 6)
      {
        enqueue(EVT_BLE_CMD_SET_START_TIMESTAMP, &data[2], 6);
      }
      break;
    case 0x04:
      enqueue(EVT_BLE_CMD_GET_TIMESTAMP, NULL, 0);
      break;
    case 0x05:
      enqueue(EVT_BLE_CMD_GET_DATA_PAGE_NO, NULL, 0);
      break;
    default:
      break;
    }
    break;
  case 0x01:
    switch (data[1])
    {
    case 0x00:
      enqueue(EVT_BLE_CMD_MEAS_START, NULL, 0);
      break;
    case 0x01:
      enqueue(EVT_BLE_CMD_MEAS_STOP, NULL, 0);
      break;
    default:
      break;
    }
    break;
  case 0x02:
    switch (data[1])
    {
    case 0x00:
      if (len >= 5)
      {
        enqueue(EVT_BLE_CMD_GET_DATA_1_DATA, &data[2], 3);
      }
      break;
    case 0x01:
      enqueue(EVT_BLE_CMD_GET_LATEST_DATA, NULL, 0);
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

MyBLE::MyBLE()
{
  this->SENSOR_Service = new BLEService(BLE_SENSOR_SERVICE_UUID);
  this->Battery_Service = new BLEService(BLE_BATTERY_SERVICE_UUID);

  this->SENSOR_TX_Chara = new MyBLECharacteristic(BLE_SENSOR_TX_CHARA_UUID);
  this->SENSOR_RX_Chara = new MyBLECharacteristic(BLE_SENSOR_RX_CHARA_UUID);
  this->Battery_chara = new MyBLECharacteristic(BLE_BATTERY_CHARA_UUID);
}

MyBLE::~MyBLE()
{
  delete this->SENSOR_Service;
  delete this->Battery_Service;
  delete this->SENSOR_TX_Chara;
  delete this->SENSOR_RX_Chara;
  delete this->Battery_chara;
}

void MyBLE::initialize()
{
  // Configure MTU size and bandwidth to allow larger data packets (e.g. 33 bytes)
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  // Initialize Bluefruit
  Bluefruit.begin();

  // Disable automatic connection LED blinking to give full control to application
  Bluefruit.autoConnLed(false);

  // Set TX Power to 0dBm as requested by user
  Bluefruit.setTxPower(0);

  Bluefruit.setName(BLE_LOCAL_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Initialize SENSOR Service
  this->SENSOR_Service->begin();

  // Initialize SENSOR TX Characteristic (Notify)
  this->SENSOR_TX_Chara->setProperties(CHR_PROPS_NOTIFY);
  this->SENSOR_TX_Chara->setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  this->SENSOR_TX_Chara->setUserDescriptor("SENSOR");
  this->SENSOR_TX_Chara->setMaxLen(33);
  this->SENSOR_TX_Chara->begin();

  // Initialize SENSOR RX Characteristic (Write)
  this->SENSOR_RX_Chara->setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  this->SENSOR_RX_Chara->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  this->SENSOR_RX_Chara->setUserDescriptor("SENSOR");
  this->SENSOR_RX_Chara->setWriteCallback(SensorCharaWrittenCallback);
  this->SENSOR_RX_Chara->setMaxLen(7);
  this->SENSOR_RX_Chara->begin();

  // Initialize Battery Service
  this->Battery_Service->begin();

  // Initialize Battery Level Characteristic (Read/Notify)
  this->Battery_chara->setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  this->Battery_chara->setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  this->Battery_chara->setUserDescriptor("Battery Level: 0 - 100");
  this->Battery_chara->setFixedLen(1);
  this->Battery_chara->begin();

  // Set initial battery value
  uint8_t batteryLevel = batterySensor->getValue();
  this->Battery_chara->write(&batteryLevel, 1);

  // Set up Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Add Services to Advertising packet
  Bluefruit.Advertising.addService(*this->SENSOR_Service);
  Bluefruit.Advertising.addService(*this->Battery_Service);

  Bluefruit.ScanResponse.addName();

  // Set advertising interval (in units of 0.625 ms)
  // Original BLE.setAdvertisingInterval(1600) means 1600 * 0.625 ms = 1000 ms.
  // In bluefruit, advertising interval is set in 0.625 ms units, so 1600 is 1000ms.
  Bluefruit.Advertising.setInterval(400, 400);
  Bluefruit.Advertising.setFastTimeout(30); // number of seconds in fast mode
  // Bluefruit.Advertising.start(0);           // 0 = Advertise continuously
}

void MyBLE::poll()
{
  // No-op for Bluefruit since it uses FreeRTOS/SoftDevice callbacks in the background
}

void MyBLE::advertiseStart()
{
  Bluefruit.Advertising.start(0);
}

void MyBLE::advertiseStop()
{
  Bluefruit.Advertising.stop();
}
