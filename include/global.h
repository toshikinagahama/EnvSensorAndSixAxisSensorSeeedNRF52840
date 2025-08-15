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
//
#define QUEUE_SIZE 10

#include <Arduino.h>
#include "MyEvent.h"
#include "MyState.h"
#include "MyBLE.h"
#include "MySensor.h"
#include "MyEnvSensor.h"
#include "MyBatterySensor.h"
#include "MyLed.h"
#include "MySys.h"

// 外部宣言
// メンバ
extern MyBLE *ble;
extern MySensor *sensor;
extern MyEnvSensor *envSensor;
extern MyBatterySensor *batterySensor;
extern MyLed *led;
extern BLEDevice central;
extern MySys *sys;
extern uint32_t CNT;
// 関数
extern void enqueue(MyEventID id, const uint8_t *payload, size_t length);
extern MyEvent dequeue();

#endif // GLOBAL_H