#include <windows.h>
#include <stdio.h>
#include <time.h>

static const int month_to_day[12] =
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

time_t
mktime (struct tm *t)
{
  short month, year;
  time_t result;

  month = t->tm_mon;
  year = t->tm_year + month / 12 + 1900;
  month %= 12;
  if (month < 0)
    {
      year -= 1;
      month += 12;
    }
  result = (year - 1970) * 365 + (year - 1969) / 4 + month_to_day[month];
  result = (year - 1970) * 365 + month_to_day[month];
  if (month <= 1)
    year -= 1;
  result += (year - 1968) / 4;
  result -= (year - 1900) / 100;
  result += (year - 1600) / 400;
  result += t->tm_mday;
  result -= 1;
  result *= 24;
  result += t->tm_hour;
  result *= 60;
  result += t->tm_min;
  result *= 60;
  result += t->tm_sec;
  return (result);
}
