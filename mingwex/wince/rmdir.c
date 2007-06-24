/*
 * rmdir.c: rmdir implementation for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public
 * Domain.  This file is a part of the mingw32ce package.  No
 * warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> 24 Jun 2007
 *
 */

#include <windows.h>
#include <io.h>

int
rmdir (const char *dirname)
{
  wchar_t dirnamew[MAX_PATH + 1];

  if (dirname == NULL)
    {
      SetLastError (ERROR_INVALID_ADDRESS);
      return -1;
    }

  mbstowcs (dirnamew, dirname, MAX_PATH);
  if (!RemoveDirectoryW (dirnamew))
    return -1;

  return 0;
}
