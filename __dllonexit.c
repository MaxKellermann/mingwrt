/*
 * dllcrt1.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Initialization code for DLLs.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>

p_atexit_fn 
__dllonexit(p_atexit_fn func, p_atexit_fn **start, p_atexit_fn **end)
{
  p_atexit_fn *tmp;
  int len;

//  TRACE("(%p,%p,%p)\n", func, start, end);

  if (!start || !*start || !end || !*end)
  {
//   FIXME("bad table\n");
   return NULL;
  }

  len = (*end - *start);

//  TRACE("table start %p-%p, %d entries\n", *start, *end, len);

  if (++len <= 0)
    return NULL;

  tmp = (p_atexit_fn *)realloc(*start, len * sizeof(tmp));
  if (!tmp)
    return NULL;
  *start = tmp;
  *end = tmp + len;
  tmp[len - 1] = func;
//  TRACE("new table start %p-%p, %d entries\n", *start, *end, len);
  return func;
}
