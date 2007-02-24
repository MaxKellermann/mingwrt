#include <time.h>

char *
ctime (const time_t *t)
{
  struct tm tms = *localtime(t);
  return asctime (&tms);
}
