#ifndef GLOBAL_H
#define GLOBAL_H

#define ADDRESS_VERSION 0                                                     // 機器のバージョン情報の先頭アドレス
#define ADDRESS_MAJOR_VERSION 0                                               // 機器のMAJORバージョン情報を格納するアドレス
#define ADDRESS_MINOR_VERSION 1                                               // 機器のMINORバージョン情報を格納するアドレス
#define ADDRESS_REVISION_VERSION 2                                            // 機器のREVISIONバージョン情報を格納するアドレス
#define ADDRESS_TIMESTAMP 4096                                                // タイムスタンプを格納するアドレス
#define ADDRESS_RESERVE 8192                                                  // タイムスタンプを格納するアドレス
#define UNIT_DATA_PAGE 64 * 1024                                              // 64Kbのデータページの単位
#define NUM_UNIT_DATA_PAGE 4                                                  // データページの数
#define NUM_DATA_SIZE 28                                                      // データサイズ
#define ADDRESS_DATA_PAGE_1 12288 + (0 * NUM_UNIT_DATA_PAGE * UNIT_DATA_PAGE) // データページ1の先頭アドレス
#define ADDRESS_DATA_PAGE_2 12288 + (1 * NUM_UNIT_DATA_PAGE * UNIT_DATA_PAGE) // データページ2の先頭アドレス
#define ADDRESS_DATA_PAGE_3 12288 + (2 * NUM_UNIT_DATA_PAGE * UNIT_DATA_PAGE) // データページ3の先頭アドレス
#define ADDRESS_DATA_PAGE_4 12288 + (3 * NUM_UNIT_DATA_PAGE * UNIT_DATA_PAGE) // データページ4の先頭アドレス
#define ADDRESS_DATA_PAGE_5 12288 + (4 * NUM_UNIT_DATA_PAGE * UNIT_DATA_PAGE) // データページ5の先頭アドレス
//
#define QUEUE_SIZE 100

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