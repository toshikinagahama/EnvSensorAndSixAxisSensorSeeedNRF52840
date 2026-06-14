#ifndef MYSTATE_H
#define MYSTATE_H

#include <Arduino.h>
#define QUEUE_SIZE 100
#define PAYLOAD_SIZE 64

enum MyState {
  STATE_WAIT,
  STATE_MEAS,
  STATE_MAX,
};

#endif // MYSTATE_H