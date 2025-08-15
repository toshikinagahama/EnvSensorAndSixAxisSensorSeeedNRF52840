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

void setup()
{
  Serial.begin(115200);
  delay(3000);
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
  timer_update();  // タイマーの更新
  button_update(); // ボタンの更新
  //
  MyEvent event = dequeue(); // イベントキューからイベントを取得
  EventHandler handler = state_transition_table[state][event.id];
  state = handler(&event.payload); // イベントハンドラを呼び出す
  delayMicroseconds(1);            // ディレイ
}