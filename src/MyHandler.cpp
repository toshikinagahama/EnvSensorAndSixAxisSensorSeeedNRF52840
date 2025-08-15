#include "MyHandler.h"
#include "MyFlashMemory.h"

ulong cnt = 0;
ulong cnt_save = 0;              // 保存用カウンタ
uint8_t data_page_no = 0;        // データページ番号
uint16_t acc_comp_sum[10] = {0}; // 0.5sの間に取得した加速度センサの値の合計
//
void init_meas()
{
  cnt = 0;
  cnt_save = 0;
  for (uint8_t i = 0; i < 10; i++)
  {
    acc_comp_sum[i] = 0;
  }
  // 初期化したいが、関数使うと長さの指定ができないので、自分で書き込む
  uint32_t add = 0;
  if (data_page_no == 0)
  {
    add = ADDRESS_DATA_PAGE_1;
  }
  else if (data_page_no == 1)
  {
    add = ADDRESS_DATA_PAGE_2;
  }
  else if (data_page_no == 2)
  {
    add = ADDRESS_DATA_PAGE_3;
  }
  else if (data_page_no == 3)
  {
    add = ADDRESS_DATA_PAGE_4;
  }
  else if (data_page_no == 4)
  {
    add = ADDRESS_DATA_PAGE_5;
  }
  for (uint32_t i = 0; i < NUM_UNIT_DATA_PAGE; i++)
  {

    QSPI_Erase(add + UNIT_DATA_PAGE * i, NRF_QSPI_ERASE_LEN_64KB);
  }
}

void stop_meas()
{
  // 測定終了
  data_page_no = (data_page_no + 1) % 5;
}

void saveToQSPI()
{
  // QSPIに保存
  // タイムスタンプ（4byte）, 加速度積算値10個(20byte), 温度（2byte）, 湿度（2byte）
  uint8_t val[NUM_DATA_SIZE];
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
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

  if (data_page_no == 0)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_1 + cnt_save * NUM_DATA_SIZE, NUM_DATA_SIZE);
  }
  else if (data_page_no == 1)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_2 + cnt_save * NUM_DATA_SIZE, NUM_DATA_SIZE);
  }
  else if (data_page_no == 2)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_3 + cnt_save * NUM_DATA_SIZE, NUM_DATA_SIZE);
  }
  else if (data_page_no == 3)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_4 + cnt_save * NUM_DATA_SIZE, NUM_DATA_SIZE);
  }
  else if (data_page_no == 4)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_5 + cnt_save * NUM_DATA_SIZE, NUM_DATA_SIZE);
  }
  cnt_save++;
  if (cnt_save >= 8640)
  {
    // 8640回保存したら、データページ番号を更新し、強制的に終了
    stop_meas();
    enqueue(EVT_BLE_CMD_MEAS_STOP, NULL, 0);
  }
}
/**
 * @brief センサの値をNotify
 *
 */
void notify()
{
  // uint8_t val[22];
  // val[0] = 0x01; // 識別子1
  // val[1] = 0x01; // 識別子2
  // val[2] = uint8_t(cnt >> 0);
  // val[3] = uint8_t(cnt >> 8);
  // val[4] = uint8_t(cnt >> 16);
  // val[5] = uint8_t(cnt >> 24);
  // val[6] = uint8_t(uint8_t(sensor->IMU->settings.accelRange >> 0));
  // val[7] = uint8_t(uint8_t(sensor->IMU->settings.accelRange >> 8));
  // val[8] = uint8_t(uint8_t(sensor->IMU->settings.gyroRange >> 0));
  // val[9] = uint8_t(uint8_t(sensor->IMU->settings.gyroRange >> 8));
  // val[10] = uint8_t(uint8_t(sensor->acc_x >> 0));
  // val[11] = uint8_t(uint8_t(sensor->acc_x >> 8));
  // val[12] = uint8_t(uint8_t(sensor->acc_y >> 0));
  // val[13] = uint8_t(uint8_t(sensor->acc_y >> 8));
  // val[14] = uint8_t(uint8_t(sensor->acc_z >> 0));
  // val[15] = uint8_t(uint8_t(sensor->acc_z >> 8));
  // val[16] = uint8_t(uint8_t(sensor->gyr_x >> 0));
  // val[17] = uint8_t(uint8_t(sensor->gyr_x >> 8));
  // val[18] = uint8_t(uint8_t(sensor->gyr_y >> 0));
  // val[19] = uint8_t(uint8_t(sensor->gyr_y >> 8));
  // val[20] = uint8_t(uint8_t(sensor->gyr_z >> 0));
  // val[21] = uint8_t(uint8_t(sensor->gyr_z >> 8));
  // ble->SENSOR_TX_Chara->writeValue(val, 22);
}

void notifyVersion()
{
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
void notifyTimestamp()
{
  uint8_t val[6];
  val[0] = 0x80; // 識別子1
  val[1] = 0x02; // 識別子2
  val[2] = uint8_t(sys->timestamp >> 0);
  val[3] = uint8_t(sys->timestamp >> 8);
  val[4] = uint8_t(sys->timestamp >> 16);
  val[5] = uint8_t(sys->timestamp >> 24);
  for (uint8_t i = 0; i < 6; i++)
  {
    Serial.print(val[i], HEX);
    Serial.print(" ");
  }
  ble->SENSOR_TX_Chara->writeValue(val, 6);
}

// EVT_NOP,
// EVT_BLE_CONNECTED,
// EVT_BLE_DISCONNECTED,
// EVT_BLE_CMD_MEAS_START,
// EVT_BLE_CMD_MEAS_STOP,
// EVT_BLE_CMD_GET_DEVICE_INFO,
// EVT_BLE_CMD_GET_START_TIMESTAMP,
// EVT_BLE_CMD_SET_START_TIMESTAMP,
// EVT_BLE_CMD_GET_DATA_1_BYTE,
// EVT_BUTTON_A_SHORT_PRESSED,
// EVT_BUTTON_A_LONG_PRESSED,
// EVT_MAX,

MyState handler_wait_nop(void *payload)
{
  // bleデバイスの状態で点滅か点灯か変える
  return STATE_WAIT;
}

MyState handler_wait_ble_connected(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_ble_disconnected(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_cmd_meas_start(void *payload)
{
  return STATE_MEAS;
}

MyState handler_wait_cmd_meas_stop(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_device_info(void *payload)
{
  notifyVersion();
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_start_timestamp(void *payload)
{
  notifyTimestamp();
  return STATE_WAIT;
}

MyState handler_wait_cmd_set_start_timestamp(void *payload)
{
  sys->setTimestamp(*((uint32_t *)payload)); // 4バイトの値を取得
  QSPI_Erase(ADDRESS_TIMESTAMP, NRF_QSPI_ERASE_LEN_4KB);
  QSPI_Write(&sys->timestamp, ADDRESS_TIMESTAMP, 4);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_data_1_data(void *payload)
{
  //  1バイトデータ取得
  {
    led->setLEDRGB(true, false, false);
    delay(10);
    led->setLEDRGB(false, true, false);
    uint8_t pBuf[24] = {0};
    uint8_t *pPayload = (uint8_t *)payload;
    // ページ番号を取得
    uint8_t pageNo = pPayload[0];                       // pPayloadの最初の1バイトがページ番号
    uint16_t dataNo = pPayload[1] | (pPayload[2] << 8); // データ番号を取得
    if (pageNo == 0)
    {
      // 本当は20だけでもいいがなぜか24byteでないと読み込めない
      nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_1 + dataNo * 20);
    }
    else if (pageNo == 1)
    {
      nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_2 + dataNo * 20);
    }
    else if (pageNo == 2)
    {
      nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_3 + dataNo * 20);
    }
    else if (pageNo == 3)
    {
      nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_4 + dataNo * 20);
    }
    else if (pageNo == 4)
    {
      nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_5 + dataNo * 20);
    }
    uint8_t val[25];                          // 識別番号2byte + ページ番号1byte + データ番号2byte + データ20byte
    val[0] = 0x82;                            // 識別子1
    val[1] = 0x00;                            // 識別子2
    val[2] = pageNo;                          // ページ番号
    val[3] = (uint8_t)(dataNo & 0xff);        // データ番号
    val[4] = (uint8_t)((dataNo >> 8) & 0xff); // データ番号
    memcpy(&val[5], pBuf, 20);
    ble->SENSOR_TX_Chara->writeValue(val, 25); // 25バイトの値を送信
  }
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_latest_data(void *payload)
{
  uint8_t val[30];
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x82;                                                                   // 識別子1
  val[1] = 0x01;                                                                   // 識別子2
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

MyState handler_wait_cmd_get_timestamp(void *payload)
{
  uint8_t val[6];
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x80;
  val[1] = 0x04;
  val[2] = uint8_t(now >> 0);
  val[3] = uint8_t(now >> 8);
  val[4] = uint8_t(now >> 16);
  val[5] = uint8_t(now >> 24);
  ble->SENSOR_TX_Chara->writeValue(val, 6);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_data_page_no(void *payload)
{
  uint8_t val[3];
  val[0] = 0x80;
  val[1] = 0x05;
  val[2] = data_page_no;
  ble->SENSOR_TX_Chara->writeValue(val, 3);
  return STATE_WAIT;
}

MyState handler_wait_button_a_short_pressed(void *payload)
{
  Serial.println("SHORT PRESSED");
  if (sys->is_set_timestamp)
  {
    // 測定へ移行
    init_meas();
    return STATE_MEAS;
  }
  else
  {
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

MyState handler_wait_button_a_long_pressed(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_timer1_timeout(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_timer2_timeout(void *payload)
{
  return STATE_WAIT;
}

MyState handler_wait_timer3_timeout(void *payload)
{
  return STATE_WAIT;
}

MyState handler_meas_nop(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_ble_connected(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_ble_disconnected(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_cmd_meas_start(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_cmd_meas_stop(void *payload)
{
  stop_meas();
  return STATE_WAIT;
}

MyState handler_meas_cmd_get_device_info(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_cmd_get_start_timestamp(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_cmd_set_start_timestamp(void *payload)
{
  return STATE_MEAS;
}
MyState handler_meas_cmd_get_data_1_data(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_cmd_get_latest_data(void *payload)
{
  uint8_t val[30];
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x82;                                                                   // 識別子1
  val[1] = 0x01;                                                                   // 識別子2
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
  return STATE_MEAS;
}

MyState handler_meas_cmd_get_timestamp(void *payload)
{
  uint8_t val[6];
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = 0x80;
  val[1] = 0x04;
  val[2] = uint8_t(now >> 0);
  val[3] = uint8_t(now >> 8);
  val[4] = uint8_t(now >> 16);
  val[5] = uint8_t(now >> 24);
  ble->SENSOR_TX_Chara->writeValue(val, 6);
  return STATE_MEAS;
}

MyState handler_meas_cmd_get_data_page_no(void *payload)
{
  uint8_t val[3];
  val[0] = 0x80;
  val[1] = 0x05;
  val[2] = data_page_no;
  ble->SENSOR_TX_Chara->writeValue(val, 3);
  return STATE_MEAS;
}

MyState handler_meas_button_a_short_pressed(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_button_a_long_pressed(void *payload)
{
  return STATE_MEAS;
}

MyState handler_meas_timer1_timeout(void *payload)
{
  // ble->Battery_chara->writeValue((batterySensor->getValue()));
  sensor->getValue();
  // Serial.print(sensor->roll);
  // Serial.print(",");
  // Serial.print(sensor->pitch);
  // Serial.print(",");
  // Serial.print(sensor->yaw);
  // Serial.println();
  // Serial.print(sensor->acc_x);
  // Serial.print(",");
  // Serial.print(sensor->acc_y);
  // Serial.print(",");
  // Serial.print(sensor->acc_z);
  // Serial.println();
  acc_comp_sum[(cnt / 5) % 10] += (uint16_t)(sensor->acc_comp * 1000); // 0.5sの間に取得した加速度センサの値の合計
  // Serial.print((uint16_t)(sensor->acc_comp * 1000));
  // Serial.println();
  //  ulong time_e_last = 0;
  //  acc_x_sum += abs(sensor->acc_x);
  //  acc_y_sum += abs(sensor->acc_y);
  //  acc_z_sum += abs(sensor->acc_z);
  cnt++;
  return STATE_MEAS;
}

MyState handler_meas_timer2_timeout(void *payload)
{
  envSensor->getValue();
  Serial.print("cnt : ");
  Serial.println(cnt);
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  Serial.print("now : ");
  Serial.println(now);
  Serial.print("acc_comp_sum : ");
  Serial.println(acc_comp_sum[0]);
  Serial.print("temp_obj : ");
  Serial.println(envSensor->tmp_obj);
  Serial.print("temp_env : ");
  Serial.println(envSensor->tmp_env);
  saveToQSPI();
  for (uint8_t i = 0; i < 10; i++)
  {
    acc_comp_sum[i] = 0;
  }
  return STATE_MEAS;
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
        handler_wait_cmd_get_data_page_no,    // EVT_BLE_CMD_GET_DATA_PAGE_NO
        handler_wait_button_a_short_pressed,  // EVT_BUTTON_A_SHORT_PRESSED
        handler_wait_button_a_long_pressed,   // EVT_BUTTON_A_LONG_PRESSED
        handler_wait_timer1_timeout,          // EVT_TIMER1_TIMEOUT
        handler_wait_timer2_timeout,          // EVT_TIMER2_TIMEOUT
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
        handler_meas_cmd_get_data_page_no,    // EVT_BLE_CMD_GET_DATA_PAGE_NO
        handler_meas_button_a_short_pressed,  // EVT_BUTTON_A_SHORT_PRESSED
        handler_meas_button_a_long_pressed,   // EVT_BUTTON_A_LONG_PRESSED
        handler_meas_timer1_timeout,          // EVT_TIMER1_TIMEOUT
        handler_meas_timer2_timeout,          // EVT_TIMER2_TIMEOUT
    }};
