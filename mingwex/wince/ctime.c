/*
 * ctime.c: ctime implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <time.h>

char *
ctime (const time_t *t)
{
  struct tm tms = *localtime(t);
  return asctime (&tms);
}
