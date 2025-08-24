#include "MyDisplay.h"
#include "global.h"
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <func.h>

MyDisplay::MyDisplay()
{
}

MyDisplay::~MyDisplay()
{
}

/**
 * @brief 初期化関数
 *
 */
void MyDisplay::initialize()
{
  this->display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
  if (!this->display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      ; // Don't proceed, loop forever
  }
  this->display->clearDisplay(); // 画面描写エリアを削除。
  this->display->setTextColor(WHITE);
  this->display->display(); // 画面描写エリアをディスプレイに転送。ここで全画面を削除。
  delay(500);               // 1秒待機

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
}

void MyDisplay::update()
{

  this->display->display(); // 画面描写エリアをディスプレイに転送。ここで全画面を削除。
  this->display->clearDisplay();
  uint32_t now = sys->timestamp + (millis() - sys->time_from_get_timstamp) / 1000; // タイムスタンプ + タイムスタンプ取得からの経過時間
  now += 60 * 60 * 9;
  //
  uint8_t battery = batterySensor->getValue(); // バッテリーセンサの値を取得
  this->display->setTextSize(1);               // フォントサイズ指定。
  this->display->setCursor(0, 0);              // 描写開始座標（X.Y）
  uint16_t day = 0;                            // 日を計算
  uint16_t month = 0;                          // 月を計算
  uint16_t year = 0;                           // 年を計算（1970年からの経過年数を計算）
  timestampToDate(now, &year, &month, &day);   // 年月日を取得
  this->display->print(year < 10 ? "0" : "");  // 年が1桁ならば0を表示
  this->display->print(year);                  // 年を表示
  this->display->print("/");
  this->display->print(month < 10 ? "0" : ""); // 月が1桁ならば0を表示
  this->display->print(month);                 // 月を表示
  this->display->print("/");
  this->display->print(day < 10 ? "0" : ""); // 日が1桁ならば0を表示
  this->display->print(day);                 // 日を表示
  this->display->print("    ");
  this->display->print(battery); // 日を表示
  this->display->print("%");

  this->display->setTextSize(1);              // フォントサイズ指定。
  this->display->setCursor(0, 8);             // 描写開始座標（X.Y）
  uint8_t hour = (now / 3600) % 24;           // 時間を計算
  uint8_t minute = (now / 60) % 60;           // 分を計算
  uint8_t second = now % 60;                  // 秒を計算
  this->display->print(hour < 10 ? "0" : ""); // 時間が1桁ならば0を表示
  this->display->print(hour);                 // 時間を表示
  this->display->print(":");
  this->display->print(minute < 10 ? "0" : ""); // 分が1桁ならば0を表示
  this->display->print(minute);                 // 分を表示
  this->display->print(":");
  this->display->print(second < 10 ? "0" : ""); // 秒が1桁ならば0を表示
  this->display->print(second);                 // 秒を表示
  // データページ番号と測定回数を表示
  this->display->setTextSize(1);   // フォントサイズ指定。
  this->display->setCursor(0, 16); // 描写開始座標（X.Y）
  this->display->print("Page:");
  this->display->print((uint16_t)(sys->data_page_no)); // データページ番号を表示
  // this->display->print("  Cnt:");
  // this->display->print((uint32_t)(sys->cnt)); // 測定回数を表示
  this->display->print("  S:");
  this->display->print((uint32_t)(sys->cnt_save)); // 保存した回数を表示

  // 環境センサの値を表示
  this->display->setTextSize(1);   // フォントサイズ指定。
  this->display->setCursor(0, 24); // 描写開始座標（X.Y）
  this->display->print("T0:");
  this->display->print(envSensor->tmp_obj); // 対象温度を表示
  this->display->print("C ");
  this->display->print("TE:");
  this->display->print(envSensor->tmp_env); // 環境温度を表示
  this->display->print("C");

  this->display->display(); // 画面描写エリアをディスプレイに転送。ここで全画面を削除。
}