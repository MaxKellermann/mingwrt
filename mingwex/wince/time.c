#include <time.h>
#include <windows.h>

time_t
time (time_t * timer)
{
  SYSTEMTIME systime;
  struct tm tmtime;
  time_t tt;

  GetLocalTime (&systime);

  tmtime.tm_year = systime.wYear - 1900;
  tmtime.tm_mon = systime.wMonth - 1;
  tmtime.tm_mday = systime.wDay;
  tmtime.tm_wday = systime.wDayOfWeek;
  tmtime.tm_hour = systime.wHour;
  tmtime.tm_min = systime.wMinute;
  tmtime.tm_sec = systime.wSecond;

  tt = mktime (&tmtime);

  if (timer)
    *timer = tt;

  return tt;
}
