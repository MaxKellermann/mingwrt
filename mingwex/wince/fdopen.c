/*
 * fdopen.c: fdopen and _fdopen implementations for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MODE 64

FILE *
_fdopen (int fildes, const char *mode)
{
  FILE *f;
  wchar_t wmode[MAX_MODE];
  size_t sizem = strlen (mode) + 1;
  if (sizem > MAX_MODE)
    return NULL;
  mbstowcs (wmode, mode, sizem);
  f = _wfdopen (fildes, wmode);
  return f;
}

FILE *
fdopen (int fildes, const char *mode)
{
  return _fdopen (fildes, mode);
}
