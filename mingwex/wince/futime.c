/*
 * futime.c: _futime implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <sys/utime.h>

#include "timeutil.h"

int 
_futime (int fildes, struct _utimbuf *times)
{
  FILETIME last_write_time;
  FILETIME last_access_time;

  struct _utimbuf times_def;
  
  if (!times)
    {
	 time_t curr_time;
	 time (&curr_time);
	 times_def.actime = times_def.modtime = curr_time;
	 times = &times_def;
    }
  
  __time_t_to_FILETIME (times->actime, &last_access_time);
  __time_t_to_FILETIME (times->modtime, &last_write_time);
  
  if (!SetFileTime ((HANDLE) fildes, 
				NULL, 
				&last_access_time,
				&last_write_time))
    return -1;

  return 0;
}
