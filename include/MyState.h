#ifndef MYSTATE_H

#ifdef MYSTATE_H
#define EXTERN extern
#else
#define EXTERN
#endif
#define MYSTATE_H

#include <Arduino.h>
#define QUEUE_SIZE 10
#define PAYLOAD_SIZE 64

enum MyState
{
  STATE_WAIT,
  STATE_MEAS,
  STATE_MAX,
};

#endif // MYSTATE_H