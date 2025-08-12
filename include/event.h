#ifndef EVENT_H

#ifdef EVENT_H
#define EXTERN extern
#else
#define EXTERN
#endif
#define EVENT_H

#include <Arduino.h>

enum MyState
{
  STATE_WAIT,
  STATE_ADVERTISE,
  STATE_MEAS,
  STATE_READ_DATA,
  STATE_MAX,
};

enum MyEvent
{
  EVT_NOP,
  EVT_BLE_CONNECTED,
  EVT_BLE_DISCONNECTED,
  EVT_BLE_CMD_MEAS_START,
  EVT_BLE_CMD_MEAS_STOP,
  EVT_BLE_CMD_GET_DEVICE_INFO,
  EVT_BLE_CMD_GET_START_TIMESTAMP,
  EVT_BLE_CMD_SET_START_TIMESTAMP,
  EVT_BLE_CMD_GET_DATA_1_BYTE,
  EVT_BUTTON_A_SHORT_PRESSED,
  EVT_BUTTON_A_LONG_PRESSED,
};

// 外部宣言
extern MyEvent Detect_Event();
extern void Release_Event(MyEvent);
#endif // EVENT_H