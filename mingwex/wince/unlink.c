#include <stdio.h>
#include <windows.h>

int
_unlink (const char *file)
{
  wchar_t wfile[MAX_PATH];
  size_t conv = mbstowcs (wfile, file, MAX_PATH);
  if (conv > 0 && conv < MAX_PATH && DeleteFileW (wfile))
    return 0;
  return -1;
}
