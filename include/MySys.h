#ifndef MYSYS_H
#define MYSYS_H
// インクルード
#include <Arduino.h>

class MySys
{

public:
  // メンバ
  uint8_t version[3] = {0x01, 0x00, 0x00}; // バージョン情報
  uint32_t timestamp = 0;                  // タイムスタンプ
  uint32_t time_from_get_timstamp = 0;     // タイムスタンプ
  bool is_set_timestamp = false;           // タイムスタンプが設定されているかどうか

  //  関数
  MySys();
  ~MySys();
  void initialize();
  void setVersion(uint8_t major, uint8_t minor, uint8_t revision);
  void setTimestamp(uint32_t ts);
};
#endif