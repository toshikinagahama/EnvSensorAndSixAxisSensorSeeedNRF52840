#include <Arduino.h>
#include <nrfx_gpiote.h>
#include <nrf52840.h>
#include "MyGlobal.h"
#include "MyTimer.h"
#include "MyHandler.h"
#include "MyButton.h"
#include "MyFlashMemory.h"
// #include <Adafruit_SSD1306.h>

MyState state;

void setVersion()
{
  uint8_t version[3] = {0, 0, 2};
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

void initTimestamp()
{
  uint32_t timestamp = 0;
  QSPI_Erase(ADDRESS_TIMESTAMP, NRF_QSPI_ERASE_LEN_4KB);
  QSPI_Write(&timestamp, ADDRESS_TIMESTAMP, sizeof(timestamp));
  QSPI_WaitForReady();
}

void getTimestamp()
{
  uint32_t *pBuf = (uint32_t *)&sys->timestamp;
  nrfx_qspi_read(pBuf, sizeof(sys->timestamp), ADDRESS_TIMESTAMP);
  Serial.print("Timestamp :");
  Serial.println(sys->timestamp);
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
  // Bluefruit handles polling automatically in the background
}

void ui_update()
{
  switch (state)
  {
  case STATE_WAIT:
    if (ble->isConnect)
    {
      if (sys->is_set_timestamp == false)
        led->setLEDRGB(false, true, false);
      else
        led->setLEDRGB(false, false, true);
    }
    else
    {
      if (sys->is_set_timestamp == false)
        led->greenBlink(200, 1000); // タイムスタンプが0ならば、グリーン点滅
      else
        led->blueBlink(200, 1000); // タイムスタンプが0でなければ、ブルー点滅
    }
    break;
  case STATE_MEAS:
    if (ble->isConnect)
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
  NRF_POWER->RESETREAS = NRF_POWER->RESETREAS;
  button_initialize(); // ボタンはクラスにしたかったが、割り込み関数は静的じゃないといけないので、関数化してる。initializeで割り込みしてる
  timer_initialize();  // タイマーの初期化
  sensor->initialize();
  envSensor->initialize();
  ble->initialize();
  ble->advertiseStart();
  flashmemory_initialize(); // フラッシュメモリの初期化
  initTimestamp();          // タイムスタンプの初期化
  setVersion();
  getTimestamp();
  // display->initialize(); // ディスプレイの初期化

  state = STATE_WAIT;
}

void loop()
{
  MyEvent event = dequeue();
  if (state < STATE_MAX && event.id < EVT_MAX)
  {
    // ガード
    EventHandler handler = state_handler_table[state][event.id]; // 状態遷移テーブルからハンドラを取得
    // 2. ハンドラがNULLでないか（ちゃんと登録されているか）をガード！
    if (handler != NULL)
    {
      // 安全が確認できたので実行し、次の状態（戻り値）で上書きする
      state = handler(&event.payload);
    }
  }
  ui_update(); // 状態に応じて表示を更新
}