#ifndef MYHANDLER_H

#ifdef MYHANDLER_H
#define MYHANDLER extern
#else
#define MYHANDLER
#endif
#define MYHANDLER_H

#include <Arduino.h>
#include "global.h"

typedef MyState (*EventHandler)(void *payload);

// 外部宣言
extern EventHandler state_transition_table[STATE_MAX][EVT_MAX];

#endif // MYHANDLER_H