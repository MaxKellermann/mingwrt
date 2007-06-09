#include "timeutil.h"

time_t
time (time_t *timer)
{
  SYSTEMTIME s;
  FILETIME f;
  time_t t;

  if (timer == NULL)
    timer = &t;

  GetSystemTime (&s);
  SystemTimeToFileTime (&s, &f);
  *timer = __FILETIME_to_time_t (&f);
  return *timer;
}
