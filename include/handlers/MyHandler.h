#ifndef MYHANDLER_H
#define MYHANDLER_H

#include "global.h"
#include <Arduino.h>

// Shared helpers
extern uint16_t acc_comp_sum[10];
void init_meas();
void stop_meas();
void saveToQSPI();
void notify();
void notifyVersion();
void notifyTimestamp();

typedef MyState (*EventHandler)(void *payload);

// 外部宣言
extern EventHandler state_transition_table[STATE_MAX][EVT_MAX];

#endif // MYHANDLER_H