#ifndef MYEVENT_H

#ifdef MYEVENT_H
#define EXTERN extern
#else
#define EXTERN
#endif
#define MYEVENT_H

#include <Arduino.h>
#define PAYLOAD_SIZE 64

enum MyEventID
{
  EVT_NOP,
  EVT_BLE_CONNECTED,
  EVT_BLE_DISCONNECTED,
  EVT_BLE_CMD_MEAS_START,
  EVT_BLE_CMD_MEAS_STOP,
  EVT_BLE_CMD_GET_DEVICE_INFO,
  EVT_BLE_CMD_GET_START_TIMESTAMP,
  EVT_BLE_CMD_SET_START_TIMESTAMP,
  EVT_BLE_CMD_GET_DATA_1_DATA,
  EVT_BLE_CMD_GET_LATEST_DATA,  // 最新のデータを取得するコマンド
  EVT_BLE_CMD_GET_TIMESTAMP,    // 現在のタイムスタンプを取得するコマンド
  EVT_BLE_CMD_GET_DATA_PAGE_NO, // 現在のデータページNOを取得するコマンド
  EVT_BUTTON_A_SHORT_PRESSED,
  EVT_BUTTON_A_LONG1_PRESSED, // 長押し1のイベント
  EVT_BUTTON_A_LONG2_PRESSED, // 長押し2のイベント（deepsleepモード用）
  EVT_TIMER1_TIMEOUT,         // タイマー1のタイムアウトイベント
  EVT_TIMER2_TIMEOUT,         // タイマー2のタイムアウトイベント
  EVT_MAX,
};

// イベント情報を格納する構造体
typedef struct
{
  MyEventID id;                  // イベントID
  uint8_t payload[PAYLOAD_SIZE]; // ペイロード（最大64バイト）
  size_t length;                 // ペイロードの長さ
  unsigned long timestamp;       // タイムスタンプ
} MyEvent;

#endif // MYEVENT_H