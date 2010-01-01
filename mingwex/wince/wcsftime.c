/*
 * wcsftime.c: wcsftime implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <string.h>
#include <stdlib.h>
#include <time.h>

size_t
wcsftime (wchar_t *const ws, const size_t maxsize,
          const wchar_t *const wformat, const struct tm * const t)
{
  size_t format_size = wcslen (wformat) + 1;
  char *format = malloc (format_size);
  char *s = malloc (maxsize);
  size_t ret;

  wcstombs (format, wformat, format_size);
  ret = strftime (s, maxsize, format, t);
  if (ret >= 0)
    ret = mbstowcs (ws, s, ret);
  free (format);
  free (s);
  return ret;
}
