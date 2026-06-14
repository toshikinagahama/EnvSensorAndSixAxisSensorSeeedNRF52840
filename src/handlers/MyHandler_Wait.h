#ifndef MYHANDLER_WAIT_H
#define MYHANDLER_WAIT_H

#include "MyState.h"

MyState handler_wait_nop(void *payload);
MyState handler_wait_ble_connected(void *payload);
MyState handler_wait_ble_disconnected(void *payload);
MyState handler_wait_cmd_meas_start(void *payload);
MyState handler_wait_cmd_meas_stop(void *payload);
MyState handler_wait_cmd_get_device_info(void *payload);
MyState handler_wait_cmd_get_start_timestamp(void *payload);
MyState handler_wait_cmd_set_start_timestamp(void *payload);
MyState handler_wait_cmd_get_data_1_data(void *payload);
MyState handler_wait_cmd_get_latest_data(void *payload);
MyState handler_wait_cmd_get_timestamp(void *payload);
MyState handler_wait_cmd_get_data_page_no(void *payload);
MyState handler_wait_button_a_short_pressed(void *payload);
MyState handler_wait_button_a_long1_pressed(void *payload);
MyState handler_wait_button_a_long2_pressed(void *payload);
MyState handler_wait_timer1_timeout(void *payload);
MyState handler_wait_timer2_timeout(void *payload);
MyState handler_wait_timer3_timeout(void *payload);

#endif // MYHANDLER_WAIT_H
