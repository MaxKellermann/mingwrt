/*
 * tempname.c: tempname, _tempname and _wtempnam implementations for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> Feb 2007
 *
 */

#include <stdio.h>
#include <windows.h>

typedef void *(*dupfun_t) (wchar_t *);

static void *
tempnam_imp (const wchar_t * dir, const wchar_t * prefix, dupfun_t dupfun)
{
  wchar_t tmpbuf[MAX_PATH];
  wchar_t tmp_dir[MAX_PATH];

  DWORD ret = GetTempPathW (MAX_PATH, tmp_dir);
  if (ret > 0 && ret <= MAX_PATH)
    {
      dir = tmp_dir;
    }

  if (!dir || !*dir || !(GetFileAttributes (dir) & FILE_ATTRIBUTE_DIRECTORY))
    {
      dir = _wP_tmpdir;
    }

  if (GetTempFileNameW (dir, prefix, 0, tmpbuf))
    {
      DeleteFileW (tmpbuf);
      return dupfun (tmpbuf);
    }
  return NULL;
}

static void *
wcstombs_dup (wchar_t * str)
{
  size_t len = wcslen (str) + 1;
  char *s = malloc (len);
  wcstombs (s, str, len);
  return s;
}

wchar_t *
_wtempnam (const wchar_t * dir, const wchar_t * prefix)
{
  return tempnam_imp (dir, prefix, (dupfun_t) wcsdup);
}

char *
_tempnam (const char *dir, const char *prefix)
{
  wchar_t wdir[MAX_PATH];
  wchar_t wprefix[MAX_PATH];
  mbstowcs (wdir, dir, MAX_PATH);
  mbstowcs (wprefix, prefix, MAX_PATH);
  return tempnam_imp (wdir, wprefix, wcstombs_dup);
}

char *
tempnam (const char *dir, const char *prefix)
{
  return _tempnam (dir, prefix);
}
