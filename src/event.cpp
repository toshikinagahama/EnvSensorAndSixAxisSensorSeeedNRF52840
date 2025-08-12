#include "event.h"
#include "global.h"

/**
 *
 * イベント検知
 */
MyEvent Detect_Event()
{
  if (INT.BIT.BLE_CONNECTED == 1)
    return EVT_BLE_CONNECTED;
  if (INT.BIT.BLE_DISCONNECTED == 1)
    return EVT_BLE_DISCONNECTED;
  if (INT.BIT.BLE_CMD_MEAS_START == 1)
    return EVT_BLE_CMD_MEAS_START;
  if (INT.BIT.BLE_CMD_MEAS_STOP == 1)
    return EVT_BLE_CMD_MEAS_STOP;
  if (INT.BIT.BLE_CMD_GET_DEVICE_INFO == 1)
    return EVT_BLE_CMD_GET_DEVICE_INFO;
  if (INT.BIT.BLE_CMD_GET_START_TIMESTAMP == 1)
    return EVT_BLE_CMD_GET_START_TIMESTAMP;
  if (INT.BIT.BLE_CMD_SET_START_TIMESTAMP == 1)
    return EVT_BLE_CMD_SET_START_TIMESTAMP;
  if (INT.BIT.BLE_CMD_GET_DATA_1_BYTE == 1)
    return EVT_BLE_CMD_GET_DATA_1_BYTE;
  if (INT.BIT.BUTTON_A_SHORT_PRESSED == 1)
    return EVT_BUTTON_A_SHORT_PRESSED;
  if (INT.BIT.BUTTON_A_LONG_PRESSED == 1)
    return EVT_BUTTON_A_LONG_PRESSED;
  return EVT_NOP;
}

/**
 * @brief イベント開放
 *
 */
void Release_Event(MyEvent EVT)
{
  switch (EVT)
  {
  case EVT_BLE_CONNECTED:
    INT.BIT.BLE_CONNECTED = 0;
    break;
  case EVT_BLE_DISCONNECTED:
    INT.BIT.BLE_DISCONNECTED = 0;
    break;
  case EVT_BLE_CMD_MEAS_START:
    INT.BIT.BLE_CMD_MEAS_START = 0;
    break;
  case EVT_BLE_CMD_MEAS_STOP:
    INT.BIT.BLE_CMD_MEAS_STOP = 0;
    break;
  case EVT_BLE_CMD_GET_DATA_1_BYTE:
    for (uint8_t i = 0; i < 64; i++)
      SYS.BLE_ARG[i] = 0; // 初期化
    INT.BIT.BLE_CMD_GET_DATA_1_BYTE = 0;
    break;
  case EVT_BLE_CMD_GET_DEVICE_INFO:
    INT.BIT.BLE_CMD_GET_DEVICE_INFO = 0;
    break;
  case EVT_BLE_CMD_GET_START_TIMESTAMP:
    INT.BIT.BLE_CMD_GET_START_TIMESTAMP = 0;
    break;
  case EVT_BLE_CMD_SET_START_TIMESTAMP:
    for (uint8_t i = 0; i < 64; i++)
      SYS.BLE_ARG[i] = 0; // 初期化
    INT.BIT.BLE_CMD_SET_START_TIMESTAMP = 0;
    break;
  case EVT_BUTTON_A_SHORT_PRESSED:
    INT.BIT.BUTTON_A_SHORT_PRESSED = 0;
    break;
  case EVT_BUTTON_A_LONG_PRESSED:
    INT.BIT.BUTTON_A_LONG_PRESSED = 0;
    break;

  default:
    break;
  }
}