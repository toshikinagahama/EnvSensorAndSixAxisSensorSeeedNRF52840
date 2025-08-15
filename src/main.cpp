#include <Arduino.h>
#include <nrf52840.h>
#include "global.h"
#include "MyTimer.h"
#include "MyHandler.h"
#include "MyButton.h"
#include "MyFlashMemory.h"

MyState state;

void setVersion()
{
  uint8_t version[3] = {0, 0, 1};
  QSPI_Erase(ADDRESS_VERSION, NRF_QSPI_ERASE_LEN_4KB);
  QSPI_Write(&version[0], ADDRESS_MAJOR_VERSION, 1);
  QSPI_Write(&version[1], ADDRESS_MINOR_VERSION, 1);
  QSPI_Write(&version[2], ADDRESS_REVISION_VERSION, 1);
  // nrfx_qspi_write(pBuf + 4, 4, offset + 4);
  QSPI_WaitForReady();

  sys->setVersion(version[0], version[1], version[2]);
  // uint8_t pBuf2[4];
  // nrfx_qspi_read(pBuf2, 4, 0x00);
  //  Serial.print("Data :");
  //  for (uint8_t i = 0; i < 3; i++)
  //{
  //    Serial.print(" 0x");
  //    Serial.print((uint8_t)(pBuf2[i]), HEX);
  //  }
  //  Serial.println("");
}

void getTimestamp()
{
  uint32_t *pBuf = (uint32_t *)&sys->timestamp;
  nrfx_qspi_read(pBuf, sizeof(sys->timestamp), ADDRESS_TIMESTAMP);
  if (sys->timestamp == 0)
  {
    sys->is_set_timestamp = false;
  }
  else
  {
    sys->is_set_timestamp = true;
  }
}

void ble_update()
{
  central = BLE.central();
  if (central && central.connected())
  {
  }
  else
  {
    ble->poll();
  }
}

void ui_update()
{
  switch (state)
  {
  case STATE_WAIT:
    if (central && central.connected())
    {
      if (sys->timestamp == 0)
        led->setLEDRGB(false, true, false);
      else
        led->setLEDRGB(false, false, true);
    }
    else
    {
      if (sys->timestamp == 0)
        led->greenBlink(200, 1000); // タイムスタンプが0ならば、グリーン点滅
      else
        led->blueBlink(200, 1000); // タイムスタンプが0でなければ、ブルー点滅
    }
    break;
  case STATE_MEAS:
    if (central && central.connected())
      led->redBlink(100, 1000);
    else
      led->redBlink(100, 2000);
    break;
  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  sys->initialize(); // システムの初期化
  batterySensor->initialize();
  led->initialize();
  button_initialize(); // ボタンはクラスにしたかったが、割り込み関数は静的じゃないといけないので、関数化してる。initializeで割り込みしてる
  ble->initialize();
  ble->advertiseStart();
  sensor->initialize();
  envSensor->initialize();
  flashmemory_initialize(); // フラッシュメモリの初期化
  timer_initialize();       // タイマーの初期化
  setVersion();
  getTimestamp();

  state = STATE_WAIT;
}

void loop()
{
  timer_update();                                                 // タイマーの更新
  button_update();                                                // ボタンの更新
  ble_update();                                                   // BLEの更新
  MyEvent event = dequeue();                                      // イベントキューからイベントを取得
  EventHandler handler = state_transition_table[state][event.id]; // 状態遷移テーブルからハンドラを取得
  state = handler(&event.payload);                                // イベントハンドラを呼び出す
  ui_update();                                                    // 状態に応じて表示を更新 ※handlerの中で表示を更新してもいいが、ここでやる
  delayMicroseconds(1);                                           // ディレイ
}