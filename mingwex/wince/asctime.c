/*
 * asctime.c: asctime implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <windows.h>
#include <stdio.h>
#include <time.h>

static const char const wday_name[7][3] =
  { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

static const char const mon_name[12][3] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

char *
asctime (const struct tm *timeptr)
{
  static char result[26];
  sprintf (result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
           wday_name[timeptr->tm_wday], mon_name[timeptr->tm_mon],
           timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min,
           timeptr->tm_sec, 1900 + timeptr->tm_year);
  return result;
}
