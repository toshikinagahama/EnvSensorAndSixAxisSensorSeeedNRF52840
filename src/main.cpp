#include <Arduino.h>
#include <nrf52840.h>
#include "global.h"
#include "MyBLE.h"
#include "MySensor.h"
#include "MyEnvSensor.h"
#include "BatterySensor.h"
#include "led.h"
#include "button.h"
#include "event.h"
#include "MyFlashMemory.h"

ulong time_from_get_timstamp = 0; // タイムスタンプ取得からの経過時間
ulong time_s0 = 0;
ulong time_s = 0;
ulong time_e = 0;
ulong time_battery_s = 0; // バッテリー送付タイミング用（1sに1回送付）
ulong cnt = 0;
ulong cnt_save = 0;       // 保存用カウンタ
uint32_t timestamp = 0;   // タイムスタンプ
uint8_t data_page_no = 0; // データページ番号
int32_t acc_x_sum = 0;    // 10msの間に取得した加速度センサの値の合計
int32_t acc_y_sum = 0;    // 10msの間に取得した加速度センサの値の合計
int32_t acc_z_sum = 0;    // 10msの間に取得した加速度センサの値の合計

MyState state;
MyBLE *ble = new MyBLE();
MySensor *sensor = new MySensor();
MyEnvSensor *envSensor = new MyEnvSensor();
BatterySensor *batterySensor = new BatterySensor();
LED *led = new LED();
BLEDevice central;

// 環境センサー

/**
 * @brief センサの値のサンプリング
 *
 */
void sampling()
{
  // 10msごとにセンサの値を取得
  // サンプリング
  sensor->getValue();
  // ulong time_e_last = 0;
  acc_x_sum += abs(sensor->acc_x);
  acc_y_sum += abs(sensor->acc_y);
  acc_z_sum += abs(sensor->acc_z);
  envSensor->getValue();
  time_e = micros();
  // 1000 = 1ms なので100000 = 100ms
  while (time_e - time_s0 < 100000 * cnt)
  {
    time_e = micros();
  }
  ulong tmp_time_s = time_s;
  time_s = time_e;
  cnt++;
}

/**
 * @brief センサの値をNotify
 *
 */
void notify()
{
  uint8_t val[22];
  val[0] = 0x01; // 識別子1
  val[1] = 0x01; // 識別子2
  val[2] = uint8_t(cnt >> 0);
  val[3] = uint8_t(cnt >> 8);
  val[4] = uint8_t(cnt >> 16);
  val[5] = uint8_t(cnt >> 24);
  val[6] = uint8_t(uint8_t(sensor->IMU->settings.accelRange >> 0));
  val[7] = uint8_t(uint8_t(sensor->IMU->settings.accelRange >> 8));
  val[8] = uint8_t(uint8_t(sensor->IMU->settings.gyroRange >> 0));
  val[9] = uint8_t(uint8_t(sensor->IMU->settings.gyroRange >> 8));
  val[10] = uint8_t(uint8_t(sensor->acc_x >> 0));
  val[11] = uint8_t(uint8_t(sensor->acc_x >> 8));
  val[12] = uint8_t(uint8_t(sensor->acc_y >> 0));
  val[13] = uint8_t(uint8_t(sensor->acc_y >> 8));
  val[14] = uint8_t(uint8_t(sensor->acc_z >> 0));
  val[15] = uint8_t(uint8_t(sensor->acc_z >> 8));
  val[16] = uint8_t(uint8_t(sensor->gyr_x >> 0));
  val[17] = uint8_t(uint8_t(sensor->gyr_x >> 8));
  val[18] = uint8_t(uint8_t(sensor->gyr_y >> 0));
  val[19] = uint8_t(uint8_t(sensor->gyr_y >> 8));
  val[20] = uint8_t(uint8_t(sensor->gyr_z >> 0));
  val[21] = uint8_t(uint8_t(sensor->gyr_z >> 8));
  ble->SENSOR_TX_Chara->writeValue(val, 22);
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
  val[2] = uint8_t(timestamp >> 0);
  val[3] = uint8_t(timestamp >> 8);
  val[4] = uint8_t(timestamp >> 16);
  val[5] = uint8_t(timestamp >> 24);
  for (uint8_t i = 0; i < 6; i++)
  {
    Serial.print(val[i], HEX);
    Serial.print(" ");
  }
  ble->SENSOR_TX_Chara->writeValue(val, 6);
}

void init_meas()
{
  time_s0 = micros();
  cnt = 0;
  cnt_save = 0;
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
  acc_x_sum = 0;
  acc_y_sum = 0;
  acc_z_sum = 0;
  cnt = 0;
  cnt_save = 0;
  time_s = 0;
  time_e = 0;
}

void saveToQSPI()
{
  // QSPIに保存
  // タイムスタンプ（4byte）, x 軸加速度積算値（4byte）, y 軸加速度積算値（4byte）, z 軸加速度積算値（4byte）, 温度（2byte）, 湿度（2byte）
  uint8_t val[20];
  uint32_t now = timestamp + (millis() - time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  val[0] = uint8_t(now >> 0);
  val[1] = uint8_t(now >> 8);
  val[2] = uint8_t(now >> 16);
  val[3] = uint8_t(now >> 24);
  val[4] = uint8_t(acc_x_sum >> 0);
  val[5] = uint8_t(acc_x_sum >> 8);
  val[6] = uint8_t(acc_x_sum >> 16);
  val[7] = uint8_t(acc_x_sum >> 24);
  val[8] = uint8_t(acc_y_sum >> 0);
  val[9] = uint8_t(acc_y_sum >> 8);
  val[10] = uint8_t(acc_y_sum >> 16);
  val[11] = uint8_t(acc_y_sum >> 24);
  val[12] = uint8_t(acc_z_sum >> 0);
  val[13] = uint8_t(acc_z_sum >> 8);
  val[14] = uint8_t(acc_z_sum >> 16);
  val[15] = uint8_t(acc_z_sum >> 24);
  int iTmp = (int)(envSensor->tmp * 100);
  int iHum = (int)(envSensor->hum * 100);
  val[16] = uint8_t(iTmp >> 0);
  val[17] = uint8_t(iTmp >> 8);
  val[18] = uint8_t(iHum >> 0);
  val[19] = uint8_t(iHum >> 8);

  if (data_page_no == 0)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_1 + cnt_save * 20, 20);
  }
  else if (data_page_no == 1)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_2 + cnt_save * 20, 20);
  }
  else if (data_page_no == 2)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_3 + cnt_save * 20, 20);
  }
  else if (data_page_no == 3)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_4 + cnt_save * 20, 20);
  }
  else if (data_page_no == 4)
  {
    QSPI_Write(val, ADDRESS_DATA_PAGE_5 + cnt_save * 20, 20);
  }
  cnt_save++;
  if (cnt_save >= 8640)
  {
    // 8640回保存したら、データページ番号を更新し、強制的に終了
    stop_meas();
    state = STATE_WAIT;
  }
}

/**
 * @brief アクション実行
 *
 * @param EVT
 */
void doAction(MyEvent EVT)
{
  switch (state)
  {
  case STATE_ADVERTISE:
    ble->poll();
    delay(100);

    if (timestamp == 0)
      led->greenBlink(200, 1000); // タイムスタンプが0ならば、グリーン点滅
    else
      led->blueBlink(200, 1000); // タイムスタンプが0でなければ、ブルー点滅
    switch (EVT)
    {
    case EVT_BLE_CONNECTED:
      state = STATE_WAIT;
      break;
    case EVT_BUTTON_A_SHORT_PRESSED:
      if (timestamp != 0)
      {
        // 測定へ移行
        init_meas();
        state = STATE_MEAS;
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
      Serial.println("SHORT PRESSED");
      break;
    case EVT_BUTTON_A_LONG_PRESSED:
      // Serial.println("Going to System OFF");
      // 間違えて押しちゃうかもしれないので、コマンドでしか終了できないようにする
      // if (timestamp != 0)
      //{
      //   Serial.println("LONG PRESSED");
      //   stop_meas();
      //   state = STATE_WAIT;
      // }
      break;
    default:
      break;
    }
    break;
  case STATE_WAIT:
    if (timestamp == 0)
      led->setLEDRGB(false, true, false);
    else
      led->setLEDRGB(false, false, true);
    central = BLE.central();
    if (central && central.connected())
    {
    }
    else
    {
      // 接続が切れた場合
      state = STATE_ADVERTISE; // 接続が切れたら、アドバタイズ状態へ
    }
    if (millis() - time_battery_s >= 1000)
    {
      // 1秒に1回
      ble->Battery_chara->writeValue((batterySensor->getValue()));
      time_battery_s = millis();
    }
    switch (EVT)
    {
    case EVT_BLE_DISCONNECTED:
      state = STATE_ADVERTISE;
      break;
    case EVT_BLE_CMD_MEAS_START:
      if (timestamp != 0)
      {
        // 測定へ移行
        init_meas();
        state = STATE_MEAS;
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
      break;
    case EVT_BLE_CMD_MEAS_STOP:
      stop_meas();
      state = STATE_WAIT;
      break;
    case EVT_BLE_CMD_GET_DEVICE_INFO:
      notifyVersion();
      break;
    case EVT_BLE_CMD_GET_START_TIMESTAMP:
      notifyTimestamp();
      break;
    case EVT_BLE_CMD_SET_START_TIMESTAMP:
      timestamp = *((uint32_t *)SYS.BLE_ARG); // 4バイトの値を取得
      QSPI_Erase(ADDRESS_TIMESTAMP, NRF_QSPI_ERASE_LEN_4KB);
      QSPI_Write(&timestamp, ADDRESS_TIMESTAMP, 4);
      time_from_get_timstamp = millis(); // たいむスタンプ取得からの経過時間をリセット
      break;
    case EVT_BLE_CMD_GET_DATA_1_BYTE:
      // 1バイトデータ取得
      {
        uint8_t pBuf[24] = {0};
        uint16_t dataNo = SYS.BLE_ARG[1] | (SYS.BLE_ARG[2] << 8); // データ番号を取得
        if (SYS.BLE_ARG[0] == 0)
        {
          // 本当は20だけでもいいがなぜか24byteでないと読み込めない
          nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_1 + dataNo * 20);
        }
        else if (SYS.BLE_ARG[0] == 1)
        {
          nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_2 + dataNo * 20);
        }
        else if (SYS.BLE_ARG[0] == 2)
        {
          nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_3 + dataNo * 20);
        }
        else if (SYS.BLE_ARG[0] == 3)
        {
          nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_4 + dataNo * 20);
        }
        else if (SYS.BLE_ARG[0] == 4)
        {
          nrfx_qspi_read(pBuf, 24, ADDRESS_DATA_PAGE_5 + dataNo * 20);
        }
        uint8_t val[25];         // 識別番号2byte + ページ番号1byte + データ番号2byte + データ20byte
        val[0] = 0x82;           // 識別子1
        val[1] = 0x00;           // 識別子2
        val[2] = SYS.BLE_ARG[0]; // ページ番号
        val[3] = SYS.BLE_ARG[1]; // データ番号
        val[4] = SYS.BLE_ARG[2]; // データ番号
        for (uint8_t i = 0; i < 20; i++)
        {
          val[i + 5] = pBuf[i]; // データをコピー
        }
        ble->SENSOR_TX_Chara->writeValue(val, 25); // 25バイトの値を送信
      }
      break;
    case EVT_BUTTON_A_SHORT_PRESSED:
      if (timestamp != 0)
      {
        // 測定へ移行
        init_meas();
        state = STATE_MEAS;
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
      Serial.println("SHORT PRESSED");
      break;
    case EVT_BUTTON_A_LONG_PRESSED:
      // 間違えて押しちゃうかもしれないので、コマンドでしか終了できないようにする
      // Serial.println("LONG PRESSED");
      // stop_meas();
      // state = STATE_WAIT;
      break;
    default:
      break;
    }
    break;
  case STATE_MEAS:
    led->redBlink(100, 2000);
    // led->setLEDRGB(true, false, false);
    sampling();
    if (central && central.connected())
    {
      // notify();
    }
    else
    {
      // 接続が切れた場合
    }
    if ((cnt - 1) % 50 == 0)
    {
      // 50回に1回、データをNotify
      // notify();
      //
      Serial.print("cnt : ");
      Serial.println(cnt);
      uint32_t now = timestamp + (millis() - time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
      Serial.print("now : ");
      Serial.println(now);
      Serial.print("acc_x_sum : ");
      Serial.println(acc_x_sum);
      Serial.print("acc_y_sum : ");
      Serial.println(acc_y_sum);
      Serial.print("acc_z_sum : ");
      Serial.println(acc_z_sum);
      Serial.print("temp : ");
      Serial.println(envSensor->tmp);
      Serial.print("hum : ");
      Serial.println(envSensor->hum);
      saveToQSPI();
      acc_x_sum = 0; // 10msの間に取得した加速度センサの値の合計をリセット
      acc_y_sum = 0; // 10msの間に取得した加速度センサの値の合計をリセット
      acc_z_sum = 0; // 10msの間に取得した加速度センサの値の合計をリセット
    }
    switch (EVT)
    {
    case EVT_BLE_CMD_MEAS_STOP:
      stop_meas();
      state = STATE_WAIT;
      break;
    case EVT_BLE_DISCONNECTED:
      // 接続が切れても測定は続けたいのでここではなにもしない
      // state = STATE_ADVERTISE;
      break;
    case EVT_BUTTON_A_SHORT_PRESSED:
      // 何もしない
      break;
    case EVT_BUTTON_A_LONG_PRESSED:
      //// 測定終了
      /// 間違えて押しちゃうかもしれないので、コマンドでしか終了できないようにする
      // stop_meas();
      // state = STATE_WAIT;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void setVersion()
{
  uint8_t version[3] = {0, 0, 1};
  QSPI_Erase(ADDRESS_VERSION, NRF_QSPI_ERASE_LEN_4KB);
  QSPI_Write(&version[0], ADDRESS_MAJOR_VERSION, 1);
  QSPI_Write(&version[1], ADDRESS_MINOR_VERSION, 1);
  QSPI_Write(&version[2], ADDRESS_REVISION_VERSION, 1);
  // nrfx_qspi_write(pBuf + 4, 4, offset + 4);
  QSPI_WaitForReady();
  uint8_t pBuf2[4];
  nrfx_qspi_read(pBuf2, 4, 0x00);
  Serial.print("Data :");
  for (uint8_t i = 0; i < 3; i++)
  {
    Serial.print(" 0x");
    Serial.print((uint8_t)(pBuf2[i]), HEX);
  }
  Serial.println("");
}

void getTimestamp()
{
  uint32_t *pBuf = (uint32_t *)&timestamp;
  nrfx_qspi_read(pBuf, sizeof(timestamp), ADDRESS_TIMESTAMP);
  Serial.print("Timestamp : 0x");
  Serial.println(timestamp, HEX);
}

void setup()
{
  Serial.begin(115200);
  batterySensor->initialize();
  led->initialize();
  button_initialize(); // ボタンはクラスにしたかったが、割り込み関数は静的じゃないといけないので、関数化してる。initializeで割り込みしてる
  ble->initialize();
  ble->advertiseStart();
  sensor->initialize();
  envSensor->initialize();
  delay(1000);
  MyFlashMemoryInitialize();
  setVersion();
  getTimestamp();

  time_s = micros();
  state = STATE_ADVERTISE;

  // BUTTON DEBUG
  led->setLEDRGB(false, false, true);
  // attachInterrupt(digitalPinToInterrupt(P1_13), handleInterrupt, CHANGE);
}

void loop()
{
  MyEvent EVT = Detect_Event(); // イベント検知
  doAction(EVT);                // イベントに応じたアクション実行
  Release_Event(EVT);           // イベント開放
  delayMicroseconds(1);         // ディレイ
}