#ifndef __TIMEUTIL_H
#define __TIMEUTIL_H

#include <windows.h>
#include <sys/time.h>

#define _onesec_in100ns 10000000LL

__CRT_INLINE long long
__FILETIME_to_ll (const FILETIME *f)
{
  long long t;
  t = (long long)f->dwHighDateTime << 32;
  t |= f->dwLowDateTime;
  return t;
}

__CRT_INLINE void
__ll_to_FILETIME (long long t, FILETIME* f)
{
  f->dwHighDateTime = (DWORD)((t >> 32) & 0x00000000FFFFFFFF);
  f->dwLowDateTime = (DWORD)(t & 0x00000000FFFFFFFF);
}

extern void __tm_to_SYSTEMTIME (struct tm *, SYSTEMTIME *);
extern void __SYSTEMTIME_to_tm (SYSTEMTIME *, struct tm *);
extern time_t __FILETIME_to_time_t (const FILETIME *);
extern void __time_t_to_FILETIME (time_t, FILETIME *);

#endif
