#include "MyHandler.h"
#include "MyFlashMemory.h"
#include "MyHandler_Meas.h"
#include "MyHandler_Wait.h"
#include <nrf_power.h>

uint16_t acc_comp_sum[10] = {0}; // 0.5sの間に取得した加速度センサの値の合計
//
void init_meas() {
  sys->cnt = 0;
  sys->cnt_save = 0;
  for (uint8_t i = 0; i < 10; i++) {
    acc_comp_sum[i] = 0;
  }
  // 初期化したいが、関数使うと長さの指定ができないので、自分で書き込む
  uint32_t add = 0;
  if (sys->data_page_no == 0) {
    add = ADDRESS_DATA_PAGE_1;
  } else if (sys->data_page_no == 1) {
    add = ADDRESS_DATA_PAGE_2;
  } else if (sys->data_page_no == 2) {
    add = ADDRESS_DATA_PAGE_3;
  } else if (sys->data_page_no == 3) {
    add = ADDRESS_DATA_PAGE_4;
  } else if (sys->data_page_no == 4) {
    add = ADDRESS_DATA_PAGE_5;
  }
  for (uint32_t i = 0; i < NUM_UNIT_DATA_PAGE; i++) {

    QSPI_Erase(add + UNIT_DATA_PAGE * i, NRF_QSPI_ERASE_LEN_64KB);
  }
}

void stop_meas() {
  // 測定終了
  sys->data_page_no = (sys->data_page_no + 1) % 5;
}

void saveToQSPI() {
  // QSPIに保存
  // タイムスタンプ（4byte）, 加速度積算値10個(20byte), 温度（2byte）,
  // 湿度（2byte）
  uint8_t val[NUM_DATA_SIZE];
  uint32_t now = sys->timestamp +
                 (millis() - sys->time_from_get_timstamp) /
                     1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = uint8_t(now >> 0);
  val[1] = uint8_t(now >> 8);
  val[2] = uint8_t(now >> 16);
  val[3] = uint8_t(now >> 24);
  val[4] = uint8_t(acc_comp_sum[0] >> 0);
  val[5] = uint8_t(acc_comp_sum[0] >> 8);
  val[6] = uint8_t(acc_comp_sum[1] >> 0);
  val[7] = uint8_t(acc_comp_sum[1] >> 8);
  val[8] = uint8_t(acc_comp_sum[2] >> 0);
  val[9] = uint8_t(acc_comp_sum[2] >> 8);
  val[10] = uint8_t(acc_comp_sum[3] >> 0);
  val[11] = uint8_t(acc_comp_sum[3] >> 8);
  val[12] = uint8_t(acc_comp_sum[4] >> 0);
  val[13] = uint8_t(acc_comp_sum[4] >> 8);
  val[14] = uint8_t(acc_comp_sum[5] >> 0);
  val[15] = uint8_t(acc_comp_sum[5] >> 8);
  val[16] = uint8_t(acc_comp_sum[6] >> 0);
  val[17] = uint8_t(acc_comp_sum[6] >> 8);
  val[18] = uint8_t(acc_comp_sum[7] >> 0);
  val[19] = uint8_t(acc_comp_sum[7] >> 8);
  val[20] = uint8_t(acc_comp_sum[8] >> 0);
  val[21] = uint8_t(acc_comp_sum[8] >> 8);
  val[22] = uint8_t(acc_comp_sum[9] >> 0);
  val[23] = uint8_t(acc_comp_sum[9] >> 8);
  int iTmpObj = (int)(envSensor->tmp_obj * 100);
  int iTmpEnv = (int)(envSensor->tmp_env * 100);
  val[24] = uint8_t(iTmpObj >> 0);
  val[25] = uint8_t(iTmpObj >> 8);
  val[26] = uint8_t(iTmpEnv >> 0);
  val[27] = uint8_t(iTmpEnv >> 8);

  if (sys->data_page_no == 0) {
    QSPI_Write(val, ADDRESS_DATA_PAGE_1 + sys->cnt_save * NUM_DATA_SIZE,
               NUM_DATA_SIZE);
  } else if (sys->data_page_no == 1) {
    QSPI_Write(val, ADDRESS_DATA_PAGE_2 + sys->cnt_save * NUM_DATA_SIZE,
               NUM_DATA_SIZE);
  } else if (sys->data_page_no == 2) {
    QSPI_Write(val, ADDRESS_DATA_PAGE_3 + sys->cnt_save * NUM_DATA_SIZE,
               NUM_DATA_SIZE);
  } else if (sys->data_page_no == 3) {
    QSPI_Write(val, ADDRESS_DATA_PAGE_4 + sys->cnt_save * NUM_DATA_SIZE,
               NUM_DATA_SIZE);
  } else if (sys->data_page_no == 4) {
    QSPI_Write(val, ADDRESS_DATA_PAGE_5 + sys->cnt_save * NUM_DATA_SIZE,
               NUM_DATA_SIZE);
  }
  sys->cnt_save++;
  if (sys->cnt_save >= 8640) {
    // 8640回保存したら、データページ番号を更新し、強制的に終了
    stop_meas();
    enqueue(EVT_BLE_CMD_MEAS_STOP, NULL, 0);
  }
}
/**
 * @brief センサの値をNotify
 *
 */
void notify() {}

void notifyVersion() {
  uint8_t val[5];
  uint8_t tmp[4] = {0, 0, 0, 0};
  nrfx_qspi_read(tmp, 4, ADDRESS_MAJOR_VERSION);
  val[0] = 0x80; // 識別子1
  val[1] = 0x01; // 識別子2
  val[2] = tmp[0];
  val[3] = tmp[1];
  val[4] = tmp[2];
  ble->SENSOR_TX_Chara->writeValue(val, sizeof(val));
}

/**
 * @brief タイムスタンプ値をNotify
 *
 */
void notifyTimestamp() {
  uint8_t val[6];
  val[0] = 0x80; // 識別子1
  val[1] = 0x02; // 識別子2
  val[2] = uint8_t(sys->timestamp >> 0);
  val[3] = uint8_t(sys->timestamp >> 8);
  val[4] = uint8_t(sys->timestamp >> 16);
  val[5] = uint8_t(sys->timestamp >> 24);
  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(val[i], HEX);
    Serial.print(" ");
  }
  ble->SENSOR_TX_Chara->writeValue(val, 6);
}

/**
 *
 * 状態遷移ハンドラテーブル
 */
EventHandler state_transition_table[STATE_MAX][EVT_MAX] = {
    // 順番間違えないように!!
    // WAIT_STATE
    {
        handler_wait_nop,                     // EVT_NOP
        handler_wait_ble_connected,           // EVT_BLE_CONNECTED
        handler_wait_ble_disconnected,        // EVT_BLE_DISCONNECTED
        handler_wait_cmd_meas_start,          // EVT_BLE_CMD_MEAS_START
        handler_wait_cmd_meas_stop,           // EVT_BLE_CMD_MEAS_STOP
        handler_wait_cmd_get_device_info,     // EVT_BLE_CMD_GET_DEVICE_INFO
        handler_wait_cmd_get_start_timestamp, // EVT_BLE_CMD_GET_START_TIMESTAMP
        handler_wait_cmd_set_start_timestamp, // EVT_BLE_CMD_SET_START_TIMESTAMP
        handler_wait_cmd_get_data_1_data,     // EVT_BLE_CMD_GET_DATA_1_DATA
        handler_wait_cmd_get_latest_data,     // EVT_BLE_CMD_GET_LATEST_DATA
        handler_wait_cmd_get_timestamp,       // EVT_BLE_CMD_GET_TIMESTAMP
        handler_wait_cmd_get_data_page_no, // EVT_BLE_CMD_GET_sys->data_page_no
        handler_wait_button_a_short_pressed, // EVT_BUTTON_A_SHORT_PRESSED
        handler_wait_button_a_long1_pressed, // EVT_BUTTON_A_LONG1_PRESSED
        handler_wait_button_a_long2_pressed, // EVT_BUTTON_A_LONG2_PRESSED
        handler_wait_timer1_timeout,         // EVT_TIMER1_TIMEOUT
        handler_wait_timer2_timeout,         // EVT_TIMER2_TIMEOUT
        handler_wait_timer3_timeout,         // EVT_TIMER3_TIMEOUT
    },

    // MEAS_STATE
    {
        handler_meas_nop,                     // EVT_NOP
        handler_meas_ble_connected,           // EVT_BLE_CONNECTED
        handler_meas_ble_disconnected,        // EVT_BLE_DISCONNECTED
        handler_meas_cmd_meas_start,          // EVT_BLE_CMD_MEAS_START
        handler_meas_cmd_meas_stop,           // EVT_BLE_CMD_MEAS_STOP
        handler_meas_cmd_get_device_info,     // EVT_BLE_CMD_GET_DEVICE_INFO
        handler_meas_cmd_get_start_timestamp, // EVT_BLE_CMD_GET_START_TIMESTAMP
        handler_meas_cmd_set_start_timestamp, // EVT_BLE_CMD_SET_START_TIMESTAMP
        handler_meas_cmd_get_data_1_data,     // EVT_BLE_CMD_GET_DATA_1_DATA
        handler_meas_cmd_get_latest_data,     // EVT_BLE_CMD_GET_LATEST_DATA
        handler_meas_cmd_get_timestamp,       // EVT_BLE_CMD_GET_TIMESTAMP
        handler_meas_cmd_get_data_page_no, // EVT_BLE_CMD_GET_sys->data_page_no
        handler_meas_button_a_short_pressed, // EVT_BUTTON_A_SHORT_PRESSED
        handler_meas_button_a_long1_pressed, // EVT_BUTTON_A_LONG1_PRESSED
        handler_meas_button_a_long2_pressed, // EVT_BUTTON_A_LONG2_PRESSED
        handler_meas_timer1_timeout,         // EVT_TIMER1_TIMEOUT
        handler_meas_timer2_timeout,         // EVT_TIMER2_TIMEOUT
        handler_meas_timer3_timeout,         // EVT_TIMER3_TIMEOUT
    }};
