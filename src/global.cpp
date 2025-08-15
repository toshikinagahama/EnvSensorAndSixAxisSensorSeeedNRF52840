#include "global.h"

/********************/
/*    内部定義      */
/********************/

MyEvent event_queue[QUEUE_SIZE]; // イベントキュー
int head = 0;
int tail = 0;
int count = 0;

// キューが空かどうかをチェック
int is_queue_empty(void)
{
  return count == 0;
}

/********************/
/*    外部定義      */
/********************/

MySys *sys = new MySys(); // システム情報
MyBLE *ble = new MyBLE();
MySensor *sensor = new MySensor();
MyEnvSensor *envSensor = new MyEnvSensor();
MyBatterySensor *batterySensor = new MyBatterySensor();
MyLed *led = new MyLed();
BLEDevice central;
static const uint8_t DEFAULT_PAYLOAD[1] = {0x00};

// キューにイベントを追加
void enqueue(MyEventID id, const uint8_t *payload, size_t length)
{
  // Serial.print("Enqueue event: ");
  // Serial.println(id);
  if (count < QUEUE_SIZE)
  {
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
    MyEvent event = event_queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    return event;
  }
  // キューが空の場合にEVT_NOPを返す
  MyEvent empty_event = {.id = EVT_NOP, .length = 0, .timestamp = 0};
  return empty_event;
}
