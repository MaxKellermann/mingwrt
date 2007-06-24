/*
 * dirent.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Derived from DIRLIB.C by Matt J. Weinstein
 * This note appears in the DIRLIB.H
 * DIRLIB.H by M. J. Weinstein   Released to public domain 1-Jan-89
 *
 * Updated by Jeremy Bettis <jeremy@hksys.com>
 * Significantly revised and rewinddir, seekdir and telldir added by Colin
 * Peters <colin@fu.is.saga-u.ac.jp>
 *
 */

#include <stdlib.h>
#ifndef __COREDLL__
#include <errno.h>
#endif
#include <string.h>
#include <io.h>
#ifndef __COREDLL__
#include <direct.h>
#endif
#include <dirent.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for GetFileAttributes */

#include <tchar.h>

#define SUFFIX	_T("*")
#define	SLASH	_T("\\")

#ifdef __COREDLL__
/* Pretty safe in this file, since we are only setting errno, and never
   reading it.  */

#define __set_errno(ERR) SetLastError (ERR)

#define ENOTDIR ERROR_DIRECTORY
#define EFAULT ERROR_INVALID_ADDRESS
#define EINVAL ERROR_INVALID_PARAMETER

#ifndef UNICODE_DIRENT

/* Windows CE is always UNICODE - work around it.  */
#undef _TDIR
#undef _TCHAR

#undef _T

#undef _topendir
#undef _treaddir
#undef _tclosedir
#undef _trewinddir
#undef _ttelldir
#undef _tseekdir

#undef _tfindfirst
#undef _tfindnext

#undef _tcscat
#undef _tcsncpy
#undef _tcslen
#undef _tcscpy
#undef _tcsrchr

#undef _tdirent

#define _TDIR DIR
#define _TCHAR CHAR

#define _T(S) S

#define _topendir opendir
#define _treaddir readdir
#define _tclosedir closedir
#define _trewinddir rewinddir
#define _ttelldir telldir
#define _tseekdir seekdir

#define _tfindfirst _findfirst
#define _tfindnext _findnext

#define _tcscat strcat
#define _tcsncpy strncpy
#define _tcslen strlen
#define _tcscpy strcpy
#define _tcsrchr strrchr

#define _tdirent dirent

#endif

#else
#define __set_errno(ERR) do { errno = (ERR); } while (0)
#endif

#if !defined (__COREDLL__)

/* Helper for opendir().  */
static inline unsigned _tGetFileAttributes (const _TCHAR * tPath)
{
#ifdef (_UNICODE)
  /* GetFileAttributesW does not work on W9x, so convert to ANSI */
  if (_osver & 0x8000)
    {
      char aPath [MAX_PATH];
      WideCharToMultiByte (CP_ACP, 0, tPath, -1, aPath, MAX_PATH, NULL,
			   NULL);
      return GetFileAttributesA (aPath);
    }
  return GetFileAttributesW (tPath);
#else /* _UNICODE */
  return GetFileAttributesA (tPath);
#endif
}

#else /* __COREDLL__ */

/* Helper for opendir().  */
static inline unsigned _tGetFileAttributes (const _TCHAR * tPath)
{
#ifdef UNICODE_DIRENT
  return GetFileAttributesW (tPath);
#else
  WCHAR wpath[MAX_PATH];
  mbstowcs (wpath, tPath, MAX_PATH);
  return GetFileAttributesW (wpath);
#endif
}

#endif

/*
 * opendir
 *
 * Returns a pointer to a DIR structure appropriately filled in to begin
 * searching a directory.
 */
_TDIR *
_topendir (const _TCHAR *szPath)
{
  _TDIR *nd;
  unsigned int rc;
  _TCHAR szFullPath[MAX_PATH];

  __set_errno (0);

  if (!szPath)
    {
      __set_errno (EFAULT);
      return (_TDIR *) 0;
    }

  if (szPath[0] == _T('\0'))
    {
      __set_errno (ENOTDIR);
      return (_TDIR *) 0;
    }

  /* Attempt to determine if the given path really is a directory. */
  rc = _tGetFileAttributes (szPath);
  if (rc == (unsigned int)-1)
    {
      /* call GetLastError for more error info */
#ifdef __COREDLL__
      /* GetFileAttributes already sets LastError.  */
#else
      errno = ENOENT;
#endif
      return (_TDIR *) 0;
    }
  if (!(rc & FILE_ATTRIBUTE_DIRECTORY))
    {
      /* Error, entry exists but not a directory. */
#ifdef __COREDLL__
      /* GetFileAttributes already sets LastError.  */
#else
      errno = ENOTDIR;
#endif
      return (_TDIR *) 0;
    }

#ifdef __COREDLL__
  /* On Windows CE paths must always be absolute.  */
  _tcsncpy (szFullPath, szPath, MAX_PATH);
#else
  /* Make an absolute pathname.  */
  _tfullpath (szFullPath, szPath, MAX_PATH);
#endif

  /* Allocate enough space to store DIR structure and the complete
   * directory path given. */
  nd = (_TDIR *) malloc (sizeof (_TDIR) + (_tcslen (szFullPath)
					   + _tcslen (SLASH)
					   + _tcslen (SUFFIX) + 1)
					  * sizeof (_TCHAR));

  if (!nd)
    {
      /* Error, out of memory. */
#ifndef __COREDLL__
      errno = ENOMEM;
#endif
      return (_TDIR *) 0;
    }

  /* Create the search expression. */
  _tcscpy (nd->dd_name, szFullPath);

  /* Add on a slash if the path does not end with one. */
  if (nd->dd_name[0] != _T('\0')
      && _tcsrchr (nd->dd_name, _T('/')) != nd->dd_name
					    + _tcslen (nd->dd_name) - 1
      && _tcsrchr (nd->dd_name, _T('\\')) != nd->dd_name
      					     + _tcslen (nd->dd_name) - 1)
    {
      _tcscat (nd->dd_name, SLASH);
    }

  /* Add on the search pattern */
  _tcscat (nd->dd_name, SUFFIX);

  /* Initialize handle to -1 so that a premature closedir doesn't try
   * to call _findclose on it. */
  nd->dd_handle = -1;

  /* Initialize the status. */
  nd->dd_stat = 0;

  /* Initialize the dirent structure. ino and reclen are invalid under
   * Win32, and name simply points at the appropriate part of the
   * findfirst_t structure. */
  nd->dd_dir.d_ino = 0;
  nd->dd_dir.d_reclen = 0;
  nd->dd_dir.d_namlen = 0;
  memset (nd->dd_dir.d_name, 0, FILENAME_MAX);

  return nd;
}


/*
 * readdir
 *
 * Return a pointer to a dirent structure filled with the information on the
 * next entry in the directory.
 */
struct _tdirent *
_treaddir (_TDIR * dirp)
{
  __set_errno (0);

  /* Check for valid DIR struct. */
  if (!dirp)
    {
      __set_errno (EFAULT);
      return (struct _tdirent *) 0;
    }

  if (dirp->dd_stat < 0)
    {
      /* We have already returned all files in the directory
       * (or the structure has an invalid dd_stat). */
      return (struct _tdirent *) 0;
    }
  else if (dirp->dd_stat == 0)
    {
      /* We haven't started the search yet. */
      /* Start the search */
      dirp->dd_handle = _tfindfirst (dirp->dd_name, &(dirp->dd_dta));

      if (dirp->dd_handle == -1)
	{
	  /* Whoops! Seems there are no files in that
	   * directory. */
	  dirp->dd_stat = -1;
	}
      else
	{
	  dirp->dd_stat = 1;
	}
    }
  else
    {
      /* Get the next search entry. */
      if (_tfindnext (dirp->dd_handle, &(dirp->dd_dta)))
	{
	  /* We are off the end or otherwise error.
	     _findnext sets errno to ENOENT if no more file
	     Undo this. */
	  DWORD winerr = GetLastError ();
	  if (winerr == ERROR_NO_MORE_FILES)
	    __set_errno (0);
	  _findclose (dirp->dd_handle);
	  dirp->dd_handle = -1;
	  dirp->dd_stat = -1;
	}
      else
	{
	  /* Update the status to indicate the correct
	   * number. */
	  dirp->dd_stat++;
	}
    }

  if (dirp->dd_stat > 0)
    {
      /* Successfully got an entry. Everything about the file is
       * already appropriately filled in except the length of the
       * file name. */
      dirp->dd_dir.d_namlen = _tcslen (dirp->dd_dta.name);
      _tcscpy (dirp->dd_dir.d_name, dirp->dd_dta.name);
      return &dirp->dd_dir;
    }

  return (struct _tdirent *) 0;
}


/*
 * closedir
 *
 * Frees up resources allocated by opendir.
 */
int
_tclosedir (_TDIR * dirp)
{
  int rc;

  __set_errno (0);
  rc = 0;

  if (!dirp)
    {
      __set_errno (EFAULT);
      return -1;
    }

  if (dirp->dd_handle != -1)
    {
      rc = _findclose (dirp->dd_handle);
    }

  /* Delete the dir structure. */
  free (dirp);

  return rc;
}

/*
 * rewinddir
 *
 * Return to the beginning of the directory "stream". We simply call findclose
 * and then reset things like an opendir.
 */
void
_trewinddir (_TDIR * dirp)
{
  __set_errno (0);

  if (!dirp)
    {
      __set_errno (EFAULT);
      return;
    }

  if (dirp->dd_handle != -1)
    {
      _findclose (dirp->dd_handle);
    }

  dirp->dd_handle = -1;
  dirp->dd_stat = 0;
}

/*
 * telldir
 *
 * Returns the "position" in the "directory stream" which can be used with
 * seekdir to go back to an old entry. We simply return the value in stat.
 */
long
_ttelldir (_TDIR * dirp)
{
  __set_errno (0);

  if (!dirp)
    {
      __set_errno (EFAULT);
      return -1;
    }
  return dirp->dd_stat;
}

/*
 * seekdir
 *
 * Seek to an entry previously returned by telldir. We rewind the directory
 * and call readdir repeatedly until either dd_stat is the position number
 * or -1 (off the end). This is not perfect, in that the directory may
 * have changed while we weren't looking. But that is probably the case with
 * any such system.
 */
void
_tseekdir (_TDIR * dirp, long lPos)
{
  __set_errno (0);

  if (!dirp)
    {
      __set_errno (EFAULT);
      return;
    }

  if (lPos < -1)
    {
      /* Seeking to an invalid position. */
      __set_errno (EINVAL);
      return;
    }
  else if (lPos == -1)
    {
      /* Seek past end. */
      if (dirp->dd_handle != -1)
	{
	  _findclose (dirp->dd_handle);
	}
      dirp->dd_handle = -1;
      dirp->dd_stat = -1;
    }
  else
    {
      /* Rewind and read forward to the appropriate index. */
      _trewinddir (dirp);

      while ((dirp->dd_stat < lPos) && _treaddir (dirp))
	;
    }
}
