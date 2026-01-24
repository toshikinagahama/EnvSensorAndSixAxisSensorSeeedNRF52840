#ifndef GLOBAL_H
#define GLOBAL_H

#include "MemoryMap.h"
//
#define QUEUE_SIZE 100

#include "MyBLE.h"
#include "MyBatterySensor.h"
#include "MyDisplay.h"
#include "MyEnvSensor.h"
#include "MyEvent.h"
#include "MyLed.h"
#include "MySensor.h"
#include "MyState.h"
#include "MySys.h"
#include <Arduino.h>

// 外部宣言
// メンバ
extern MyBLE *ble;
extern MySensor *sensor;
extern MyEnvSensor *envSensor;
extern MyBatterySensor *batterySensor;
extern MyLed *led;
extern MyDisplay *display;
extern BLEDevice central;
extern MySys *sys;
extern uint32_t CNT;
// 関数
extern void enqueue(MyEventID id, const uint8_t *payload, size_t length);
extern MyEvent dequeue();

#endif // GLOBAL_H