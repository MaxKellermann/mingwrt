#include "timeutil.h"

time_t
time (time_t *timer)
{
  SYSTEMTIME s;
  FILETIME f;

  if (timer == NULL)
    return 0;

  GetSystemTime (&s);
  SystemTimeToFileTime (&s, &f);
  *timer = __FILETIME_to_time_t (&f);
  return *timer;
}
