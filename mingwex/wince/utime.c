/*
 * utime.c: utime and _utime implementations for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <sys/utime.h>
#include <fcntl.h>

int 
utime (const char *path, struct utimbuf *times)
{
  int retval;
  int fildes = open (path, _O_RDWR);
  if (fildes == -1)
    return -1;

  retval = _futime (fildes, (struct _utimbuf*)times);

  close (fildes);
  return retval;
}

int
_utime (const char *fname, struct _utimbuf *times)
{
  return utime (fname, (struct utimbuf *)times);
}
