#ifndef FUNC_H
#define FUNC_H

// うるう年を判定する関数
// 4で割り切れ、かつ100で割り切れない年、または400で割り切れる年がうるう年
int isLeapYear(uint16_t year)
{
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// タイムスタンプから日付を計算する関数
void timestampToDate(uint32_t timestamp, uint16_t *year, uint16_t *month, uint16_t *day)
{
  uint32_t days = timestamp / 86400; // 秒を日数に変換
  int current_year = 1970;

  // 年を計算
  while (1)
  {
    int days_in_year = isLeapYear(current_year) ? 366 : 365;
    if (days < days_in_year)
    {
      break;
    }
    days -= days_in_year;
    current_year++;
  }

  *year = current_year;

  // 月を計算
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (isLeapYear(current_year))
  {
    days_in_month[1] = 29; // うるう年の2月は29日
  }

  int current_month = 1;
  while (1)
  {
    if (days < days_in_month[current_month - 1])
    {
      break;
    }
    days -= days_in_month[current_month - 1];
    current_month++;
  }

  *month = current_month;
  *day = days + 1; // 0から始まるので、1を加える
}

#endif // FUNC_H