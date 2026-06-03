#include "MyGlobal.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

/********************/
/*    内部定義      */
/********************/

static QueueHandle_t xEventQueue = NULL;

static void init_event_queue()
{
  if (xEventQueue == NULL)
  {
    xEventQueue = xQueueCreate(QUEUE_SIZE, sizeof(MyEvent));
  }
}

// キューが空かどうかをチェック
int is_queue_empty(void)
{
  init_event_queue();
  return uxQueueMessagesWaiting(xEventQueue) == 0;
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
MyDisplay *display = new MyDisplay(); // ディスプレイ
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
  init_event_queue();

  if (id != 15)
  {
    Serial.print("Enqueue event: ");
    Serial.println(getEventName(id));
  }

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

  // Check if we are in an Interrupt Service Routine (ISR)
  if (__get_IPSR() != 0)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR(xEventQueue, &event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
  else
  {
    xQueueSendToBack(xEventQueue, &event, portMAX_DELAY);
  }
}

// キューからイベントを取り出す (イベントが空ならFreeRTOSにより呼び出し元のタスクが休止してCPUがスリープに入る)
MyEvent dequeue(void)
{
  init_event_queue();
  MyEvent event;

  // portMAX_DELAYを設定することで、イベントが入るまで無期限でブロック（スリープ）する
  if (xQueueReceive(xEventQueue, &event, portMAX_DELAY) == pdPASS)
  {
    return event;
  }

  // フォールバック
  MyEvent empty_event;
  empty_event.id = EVT_NOP;
  empty_event.length = 0;
  empty_event.timestamp = 0;
  return empty_event;
}
