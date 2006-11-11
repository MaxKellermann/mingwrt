#include <stdio.h>
#include <string.h>

#define MAX_MODE 64

FILE *
fdopen (int fildes, const char *mode)
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
