/*
 * chmod.c: chmod implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <wchar.h>
#include <windows.h>
#include <io.h>
#include <sys/stat.h>

int
chmod (const char *path, int mode)
{
  wchar_t pathw[MAX_PATH + 1];
  DWORD attr;

  mbstowcs (pathw, path, MAX_PATH);

  /* This is racy, but it is the best we can do.  */

  attr = GetFileAttributes (pathw);
  if(attr == 0xffffffff)
    return -1;

  if((mode & _S_IWRITE) != 0)
    attr &= ~FILE_ATTRIBUTE_READONLY;
  else
    attr |= FILE_ATTRIBUTE_READONLY;

  SetFileAttributes (pathw, attr);
  return 0;
}
