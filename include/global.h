#ifndef GLOBAL_H
#define GLOBAL_H

#define ADDRESS_VERSION 0                               // 機器のバージョン情報の先頭アドレス
#define ADDRESS_MAJOR_VERSION 0                         // 機器のMAJORバージョン情報を格納するアドレス
#define ADDRESS_MINOR_VERSION 1                         // 機器のMINORバージョン情報を格納するアドレス
#define ADDRESS_REVISION_VERSION 2                      // 機器のREVISIONバージョン情報を格納するアドレス
#define ADDRESS_TIMESTAMP 4096                          // タイムスタンプを格納するアドレス
#define ADDRESS_RESERVE 8192                            // タイムスタンプを格納するアドレス
#define UNIT_DATA_PAGE 64 * 1024                        // 64Kbのデータページの単位
#define NUM_UNIT_DATA_PAGE 3                            // データページの数
#define ADDRESS_DATA_PAGE_1 12288 + (0 * 3 * 64 * 1024) // データページ1の先頭アドレス
#define ADDRESS_DATA_PAGE_2 12288 + (1 * 3 * 64 * 1024) // データページ2の先頭アドレス
#define ADDRESS_DATA_PAGE_3 12288 + (2 * 3 * 64 * 1024) // データページ3の先頭アドレス
#define ADDRESS_DATA_PAGE_4 12288 + (3 * 3 * 64 * 1024) // データページ4の先頭アドレス
#define ADDRESS_DATA_PAGE_5 12288 + (4 * 3 * 64 * 1024) // データページ5の先頭アドレス

#include <Arduino.h>

/**
 * @brief 割り込みビット
 *
 */
struct INT_BIT_T
{
  uint8_t BLE_CONNECTED = 0;               // BLE端末と接続
  uint8_t BLE_DISCONNECTED = 0;            // BLE端末と切断
  uint8_t BLE_CMD_MEAS_START = 0;          // 測定開始コマンド
  uint8_t BLE_CMD_MEAS_STOP = 0;           // 測定終了コマンド
  uint8_t BLE_CMD_GET_DATA_1_BYTE = 0;     // 1バイトデータ取得コマンド
  uint8_t BLE_CMD_GET_DEVICE_INFO = 0;     // 機器情報取得コマンド
  uint8_t BLE_CMD_GET_START_TIMESTAMP = 0; // スタートタイムスタンプ取得コマンド
  uint8_t BLE_CMD_SET_START_TIMESTAMP = 0; // スタートタイムスタンプ設定コマンド
  uint8_t BUTTON_A_SHORT_PRESSED = 0;      // ボタンA単押し
  uint8_t BUTTON_A_LONG_PRESSED = 0;       // ボタンA長押し
};

/**
 * @brief 割り込み構造体
 *
 */
struct INT_T
{
  struct INT_BIT_T BIT; // ビットフィールド
};

/**
 * @brief システム構造体
 *
 */
struct SYS_T
{
  String DEVICE_SERIAL_NUM = "";
  String DEVICE_VERSION = "";
  uint8_t BLE_ARG[64] = {0}; // BLEコマンドの引数（最大64文字）
};

// 外部宣言
extern INT_T INT;
extern SYS_T SYS;

#endif // GLOBAL_H