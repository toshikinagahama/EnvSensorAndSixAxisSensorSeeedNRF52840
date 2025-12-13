#include "MyHandler_Wait.h"
#include "MyFlashMemory.h"
#include "MyHandler.h"
#include "global.h"
#include <nrf_power.h>

MyState handler_wait_nop(void *payload) {
  // bleデバイスの状態で点滅か点灯か変える
  return STATE_WAIT;
}

MyState handler_wait_ble_connected(void *payload) { return STATE_WAIT; }

MyState handler_wait_ble_disconnected(void *payload) { return STATE_WAIT; }

MyState handler_wait_cmd_meas_start(void *payload) { return STATE_MEAS; }

MyState handler_wait_cmd_meas_stop(void *payload) { return STATE_WAIT; }

MyState handler_wait_cmd_get_device_info(void *payload) {
  notifyVersion();
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_start_timestamp(void *payload) {
  notifyTimestamp();
  return STATE_WAIT;
}

MyState handler_wait_cmd_set_start_timestamp(void *payload) {
  sys->setTimestamp(*((uint32_t *)payload)); // 4バイトの値を取得
  QSPI_Erase(ADDRESS_TIMESTAMP, NRF_QSPI_ERASE_LEN_4KB);
  QSPI_Write(&sys->timestamp, ADDRESS_TIMESTAMP, 4);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_data_1_data(void *payload) {
  //  1バイトデータ取得
  {
    led->setLEDRGB(true, false, false);
    delay(10);
    led->setLEDRGB(false, true, false);
    uint8_t pBuf[28] = {0};
    uint8_t *pPayload = (uint8_t *)payload;
    // ページ番号を取得
    uint8_t pageNo = pPayload[0]; // pPayloadの最初の1バイトがページ番号
    uint16_t dataNo = pPayload[1] | (pPayload[2] << 8); // データ番号を取得
    if (pageNo == 0) {
      nrfx_qspi_read(pBuf, 28, ADDRESS_DATA_PAGE_1 + dataNo * NUM_DATA_SIZE);
    } else if (pageNo == 1) {
      nrfx_qspi_read(pBuf, 28, ADDRESS_DATA_PAGE_2 + dataNo * NUM_DATA_SIZE);
    } else if (pageNo == 2) {
      nrfx_qspi_read(pBuf, 28, ADDRESS_DATA_PAGE_3 + dataNo * NUM_DATA_SIZE);
    } else if (pageNo == 3) {
      nrfx_qspi_read(pBuf, 28, ADDRESS_DATA_PAGE_4 + dataNo * NUM_DATA_SIZE);
    } else if (pageNo == 4) {
      nrfx_qspi_read(pBuf, 28, ADDRESS_DATA_PAGE_5 + dataNo * NUM_DATA_SIZE);
    }
    uint8_t val[33]; // 識別番号2byte + ページ番号1byte + データ番号2byte +
                     // データ20byte
    val[0] = 0x82;                            // 識別子1
    val[1] = 0x00;                            // 識別子2
    val[2] = pageNo;                          // ページ番号
    val[3] = (uint8_t)(dataNo & 0xff);        // データ番号
    val[4] = (uint8_t)((dataNo >> 8) & 0xff); // データ番号
    memcpy(&val[5], pBuf, NUM_DATA_SIZE);
    ble->SENSOR_TX_Chara->writeValue(val, 33); // 33バイトの値を送信
  }
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_latest_data(void *payload) {
  uint8_t val[30];
  uint32_t now = sys->timestamp +
                 (millis() - sys->time_from_get_timstamp) /
                     1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x82;           // 識別子1
  val[1] = 0x01;           // 識別子2
  val[2] = uint8_t(now >> 0);
  val[3] = uint8_t(now >> 8);
  val[4] = uint8_t(now >> 16);
  val[5] = uint8_t(now >> 24);
  val[6] = uint8_t(acc_comp_sum[0] >> 0);
  val[7] = uint8_t(acc_comp_sum[0] >> 8);
  val[8] = uint8_t(acc_comp_sum[1] >> 0);
  val[9] = uint8_t(acc_comp_sum[1] >> 8);
  val[10] = uint8_t(acc_comp_sum[2] >> 0);
  val[11] = uint8_t(acc_comp_sum[2] >> 8);
  val[12] = uint8_t(acc_comp_sum[3] >> 0);
  val[13] = uint8_t(acc_comp_sum[3] >> 8);
  val[14] = uint8_t(acc_comp_sum[4] >> 0);
  val[15] = uint8_t(acc_comp_sum[4] >> 8);
  val[16] = uint8_t(acc_comp_sum[5] >> 0);
  val[17] = uint8_t(acc_comp_sum[5] >> 8);
  val[18] = uint8_t(acc_comp_sum[6] >> 0);
  val[19] = uint8_t(acc_comp_sum[6] >> 8);
  val[20] = uint8_t(acc_comp_sum[7] >> 0);
  val[21] = uint8_t(acc_comp_sum[7] >> 8);
  val[22] = uint8_t(acc_comp_sum[8] >> 0);
  val[23] = uint8_t(acc_comp_sum[8] >> 8);
  val[24] = uint8_t(acc_comp_sum[9] >> 0);
  val[25] = uint8_t(acc_comp_sum[9] >> 8);
  int iTmpObj = (int)(envSensor->tmp_obj * 100);
  int iTmpEnv = (int)(envSensor->tmp_env * 100);
  val[26] = uint8_t(iTmpObj >> 0);
  val[27] = uint8_t(iTmpObj >> 8);
  val[28] = uint8_t(iTmpEnv >> 0);
  val[29] = uint8_t(iTmpEnv >> 8);
  ble->SENSOR_TX_Chara->writeValue(val, 30); // 30バイトの値を送信
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_timestamp(void *payload) {
  uint8_t val[6];
  uint32_t now = sys->timestamp +
                 (millis() - sys->time_from_get_timstamp) /
                     1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x80;
  val[1] = 0x04;
  val[2] = uint8_t(now >> 0);
  val[3] = uint8_t(now >> 8);
  val[4] = uint8_t(now >> 16);
  val[5] = uint8_t(now >> 24);
  ble->SENSOR_TX_Chara->writeValue(val, 6);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_data_page_no(void *payload) {
  uint8_t val[3];
  val[0] = 0x80;
  val[1] = 0x05;
  val[2] = sys->data_page_no;
  ble->SENSOR_TX_Chara->writeValue(val, 3);
  return STATE_WAIT;
}

MyState handler_wait_button_a_short_pressed(void *payload) {
  Serial.println("SHORT PRESSED");
  if (sys->is_set_timestamp) {
    // 測定へ移行
    init_meas();
    return STATE_MEAS;
  } else {
    // もしタイムスタンプが0ならば、LEDを点滅させる
    led->setLEDRGB(false, true, false);
    delay(100);
    led->setLEDRGB(false, false, false);
    delay(100);
    led->setLEDRGB(false, true, false);
    delay(100);
    led->setLEDRGB(false, false, false);
    delay(100);
    led->setLEDRGB(false, true, false);
    delay(100);
    led->setLEDRGB(false, false, false);
    delay(100);
  }
  return STATE_WAIT;
}

MyState handler_wait_button_a_long1_pressed(void *payload) {
  return STATE_WAIT;
}

MyState handler_wait_button_a_long2_pressed(void *payload) {
  // deepsleepモードへ移行
  NRF_POWER->SYSTEMOFF = 1;
  return STATE_WAIT;
}

MyState handler_wait_timer1_timeout(void *payload) { return STATE_WAIT; }

MyState handler_wait_timer2_timeout(void *payload) { return STATE_WAIT; }

MyState handler_wait_timer3_timeout(void *payload) {
  envSensor->getValue();
  display->update();
  return STATE_WAIT;
}
