#include "MySys.h"

MySys::MySys()
{
}

MySys::~MySys()
{
}

/**
 * @brief 初期化関数
 *
 */
void MySys::initialize()
{
  this->version[0] = 0x00;
  this->version[1] = 0x00;
  this->version[2] = 0x00;
  this->timestamp = 0;
  this->time_from_get_timstamp = 0;
  this->is_set_timestamp = false;
  this->data_page_no = 0;
  this->cnt = 0;
  this->cnt_save = 0; // 保存用カウンタ
  // タイムスタンプを取得していないので、タイムスタンプは
}
void MySys::setVersion(uint8_t major, uint8_t minor, uint8_t revision)
{
  this->version[0] = major;
  this->version[1] = minor;
  this->version[2] = revision;
}
void MySys::setTimestamp(uint32_t ts)
{
  this->timestamp = ts;
  this->time_from_get_timstamp = millis();
  this->is_set_timestamp = true;
}
