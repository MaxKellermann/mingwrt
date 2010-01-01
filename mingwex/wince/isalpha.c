/*
 * isalpha.c: isalpha implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <ctype.h>

int
isalpha(int c)
{
  return __ISCTYPE (c, _ALPHA);
}
