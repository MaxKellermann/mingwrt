/*
 * findfind.c: _findfirst, _findnext, _findclose and the wide
 * counterparts implementations for WinCE.
 *
 * This file has no copyright assigned and is placed in the Public
 * Domain.  This file is a part of the mingw32ce package.  No
 * warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Written by Pedro Alves <pedro_alves@portugalmail.pt> 24 Jun 2007
 *
 */

#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include <io.h>
#include "timeutil.h"

static void
wfd_to_fd (const WIN32_FIND_DATAW *wfd, struct _wfinddata_t *fd)
{
  /* Map win32 file attributes to _finddata_t types.  The constants
     are numerically equal, meaning we could also:
     fd->attrib = wfd->dwFileAttributes;
     But that would copy too much.
     TODO: _A_VOLID isn't mapped.  */

  fd->attrib = 0;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
    fd->attrib |= _A_NORMAL;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    fd->attrib |= _A_RDONLY;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    fd->attrib |= _A_HIDDEN;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
    fd->attrib |= _A_SYSTEM;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    fd->attrib |= _A_SUBDIR;
  if (wfd->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
    fd->attrib |= _A_ARCH;

  fd->size = wfd->nFileSizeLow;
  wcscpy (fd->name, wfd->cFileName);

  fd->time_access = __FILETIME_to_time_t (&wfd->ftLastAccessTime);
  fd->time_create = __FILETIME_to_time_t (&wfd->ftCreationTime);
  fd->time_write = __FILETIME_to_time_t (&wfd->ftLastWriteTime);
}

static void
tofinddataa (struct _finddata_t *fda, const struct _wfinddata_t *fdw)
{
#define COPY_MEMBER(MEM) do { fda->MEM = fdw->MEM; } while (0)
  COPY_MEMBER (attrib);
  COPY_MEMBER (time_create);
  COPY_MEMBER (time_access);
  COPY_MEMBER (time_write);
  COPY_MEMBER (size);
#undef COPY_MEMBER
  wcstombs (fda->name, fdw->name, FILENAME_MAX);
};

int
_findclose (long h)
{
  if (!FindClose ((HANDLE) h))
    return -1;
  return 0;
}

long
_wfindfirst (const wchar_t *filespec, struct _wfinddata_t *f)
{
  WIN32_FIND_DATAW find_data;
  HANDLE h;

  h  = FindFirstFileW (filespec, &find_data);
  if (h == INVALID_HANDLE_VALUE)
    return -1;

  wfd_to_fd (&find_data, f);
  return (long) h;
}

long
_findfirst (const char *filespec, struct _finddata_t *f)
{
  wchar_t wfilespec[MAX_PATH];
  struct _wfinddata_t wf;
  long h;

  mbstowcs (wfilespec, filespec, MAX_PATH);

  h = _wfindfirst (wfilespec, &wf);

  if (h == -1)
    return -1;

  tofinddataa (f, &wf);
  return h;
}

int
_wfindnext (long h, struct _wfinddata_t *f)
{
  WIN32_FIND_DATAW find_data;

  if (!FindNextFileW ((HANDLE) h, &find_data))
    return -1;

  wfd_to_fd (&find_data, f);
  return 0;
}

int
_findnext (long h, struct _finddata_t * f)
{
  struct _wfinddata_t wf;
  int ret;

  ret = _wfindnext (h, &wf);

  if (ret == -1)
    return -1;

  tofinddataa (f, &wf);
  return h;
}
