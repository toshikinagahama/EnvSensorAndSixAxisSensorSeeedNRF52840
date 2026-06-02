#include "global.h"

/********************/
/*    内部定義      */
/********************/

MyEvent event_queue[QUEUE_SIZE]; // イベントキュー
int head = 0;
int tail = 0;
int count = 0;

// キューが空かどうかをチェック
int is_queue_empty(void) { return count == 0; }

/********************/
/*    外部定義      */
/********************/

MySys *sys = new MySys(); // システム情報
MyBLE *ble = new MyBLE();
MySensor *sensor = new MySensor();
MyEnvSensor *envSensor = new MyEnvSensor();
MyBatterySensor *batterySensor = new MyBatterySensor();
MyLed *led = new MyLed();
MyDisplay *display = new MyDisplay(); // ディスプレイ
BLEDevice central;
static const uint8_t DEFAULT_PAYLOAD[1] = {0x00};

const char *getEventName(MyEventID id)
{
  switch (id)
  {
  case EVT_NOP:
    return "EVT_NOP";
  case EVT_BLE_CONNECTED:
    return "EVT_BLE_CONNECTED";
  case EVT_BLE_DISCONNECTED:
    return "EVT_BLE_DISCONNECTED";
  case EVT_BLE_CMD_MEAS_START:
    return "EVT_BLE_CMD_MEAS_START";
  case EVT_BLE_CMD_MEAS_STOP:
    return "EVT_BLE_CMD_MEAS_STOP";
  case EVT_BLE_CMD_GET_DEVICE_INFO:
    return "EVT_BLE_CMD_GET_DEVICE_INFO";
  case EVT_BLE_CMD_GET_START_TIMESTAMP:
    return "EVT_BLE_CMD_GET_START_TIMESTAMP";
  case EVT_BLE_CMD_SET_START_TIMESTAMP:
    return "EVT_BLE_CMD_SET_START_TIMESTAMP";
  case EVT_BLE_CMD_GET_DATA_1_DATA:
    return "EVT_BLE_CMD_GET_DATA_1_DATA";
  case EVT_BLE_CMD_GET_LATEST_DATA:
    return "EVT_BLE_CMD_GET_LATEST_DATA";
  case EVT_BLE_CMD_GET_TIMESTAMP:
    return "EVT_BLE_CMD_GET_TIMESTAMP";
  case EVT_BLE_CMD_GET_DATA_PAGE_NO:
    return "EVT_BLE_CMD_GET_DATA_PAGE_NO";
  case EVT_BUTTON_A_SHORT_PRESSED:
    return "EVT_BUTTON_A_SHORT_PRESSED";
  case EVT_BUTTON_A_LONG1_PRESSED:
    return "EVT_BUTTON_A_LONG1_PRESSED";
  case EVT_BUTTON_A_LONG2_PRESSED:
    return "EVT_BUTTON_A_LONG2_PRESSED";
  case EVT_TIMER1_TIMEOUT:
    return "EVT_TIMER1_TIMEOUT";
  case EVT_TIMER2_TIMEOUT:
    return "EVT_TIMER2_TIMEOUT";
  case EVT_TIMER3_TIMEOUT:
    return "EVT_TIMER3_TIMEOUT";
  case EVT_MAX:
    return "EVT_MAX";
  default:
    return "UNKNOWN_EVENT";
  }
}

// キューにイベントを追加
void enqueue(MyEventID id, const uint8_t *payload, size_t length)
{

  if (id != 15)
  {
    Serial.print("Enqueue event: ");
    Serial.println(getEventName(id));
  }
  if (count < QUEUE_SIZE)
  {
    noInterrupts(); // Enter Critical Section
    MyEvent event;
    event.id = id;
    if (payload == NULL)
    {
      memcpy(event.payload, DEFAULT_PAYLOAD, 1);
    }
    else
    {
      memcpy(event.payload, payload, length);
    }
    event.length = length;
    event.timestamp = millis(); // タイムスタンプを現在のミリ秒に設定
    event_queue[tail] = event;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
    interrupts(); // Exit Critical Section
  }
  else
  {
    Serial.println("Queue is full!");
  }
}

// キューからイベントを取り出す
MyEvent dequeue(void)
{
  if (count > 0)
  {
    noInterrupts(); // Enter Critical Section
    MyEvent event = event_queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    interrupts(); // Exit Critical Section
    return event;
  }
  // キューが空の場合にEVT_NOPを返す
  MyEvent empty_event = {.id = EVT_NOP, .length = 0, .timestamp = 0};
  return empty_event;
}
