#include <windows.h>
#include <stdio.h>
#include <time.h>

static struct tm mytm;

static const int DMonth[13] =
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

static const int monthCodes[12] = { 6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

static int
calc_day_of_week (const struct tm *nTM)
{
  int day;

  day = (nTM->tm_year % 100);
  day += day / 4;
  day += monthCodes[nTM->tm_mon];
  day += nTM->tm_mday;
  while (day >= 7)
    day -= 7;

  return day;
}

struct tm *
gmtime (const time_t * timer)
{
  unsigned long x = *timer;
  int imin, ihrs, iday, iyrs;
  int sec, min, hrs, day, mon, yrs;
  int lday, qday, jday, mday;

  imin = x / 60;                       // whole minutes since 1/1/70
  sec = x - (60 * imin);               // leftover seconds
  ihrs = imin / 60;                    // whole hours since 1/1/70
  min = imin - 60 * ihrs;              // leftover minutes
  iday = ihrs / 24;                    // whole days since 1/1/70
  hrs = ihrs - 24 * iday;              // leftover hours
  iday = iday + 365 + 366;             // whole days since 1/1/68
  lday = iday / ((4 * 365) + 1);       // quadyr = 4 yr period = 1461 days
  qday = iday % ((4 * 365) + 1);       // days since current quadyr began
  if (qday >= (31 + 29))               // if past feb 29 then
    lday = lday + 1;                   // add this quadyr’s leap day to the
                                       // # of quadyrs (leap days) since 68
  iyrs = (iday - lday) / 365;          // whole years since 1968
  jday = iday - (iyrs * 365) - lday;   // days since 1 /1 of current year.
  if (qday <= 365 && qday >= 60)       // if past 2/29 and a leap year then
    jday = jday + 1;                   // add a leap day to the # of whole
                                       // days since 1/1 of current year
  yrs = iyrs + 1968;                   // compute year
  mon = 13;                            // estimate month ( +1)
  mday = 366;                          // max days since 1/1 is 365
  while (jday < mday)                  // mday = # of days passed from 1/1
    {                                  // until first day of current month
      mon = mon - 1;                   // mon = month (estimated)
      mday = DMonth[mon];              // # elapsed days at first of mon
      if ((mon > 2) && (yrs % 4) == 0) // if past 2/29 and leap year then
        mday = mday + 1;               // add leap day
                                       // compute month by decrementing
    }                                  // month until found

  day = jday - mday + 1;               // compute day of month

  mytm.tm_sec = sec;
  mytm.tm_min = min;
  mytm.tm_hour = hrs;
  mytm.tm_mday = day;
  mytm.tm_mon = mon;
  mytm.tm_year = yrs - 1900;

  mytm.tm_wday = calc_day_of_week (&mytm);
  mytm.tm_yday = jday;
  mytm.tm_isdst = 0;

  return &mytm;
}
