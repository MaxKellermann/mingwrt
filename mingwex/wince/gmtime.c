#include "timeutil.h"

struct tm *
gmtime(const time_t *t)
{
  FILETIME f;
  SYSTEMTIME s;
  static struct tm tms;
	
  __time_t_to_FILETIME (*t, &f);
  FileTimeToSystemTime (&f, &s);
  __SYSTEMTIME_to_tm (&s, &tms);
  return &tms;
}
