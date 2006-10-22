#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MODE 64

FILE *
freopen (const char *__restrict__ filename,
	 const char *__restrict__ mode, FILE * __restrict__ stream)
{
  size_t sizef = strlen (filename) + 1;
  size_t sizem = strlen (mode) + 1;

  wchar_t wfilename[MAX_PATH];
  wchar_t wmode[MAX_MODE];

  if (sizem > MAX_MODE || sizef > MAX_PATH)
    return NULL;

  mbstowcs (wfilename, filename, sizef);
  mbstowcs (wmode, mode, sizem);

  return _wfreopen (wfilename, wmode, stream);
}
