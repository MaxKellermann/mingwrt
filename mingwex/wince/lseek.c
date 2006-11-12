#include <windows.h>
#include <unistd.h>

long
_lseek (int fildes, long offset, int whence)
{
  DWORD mode = 0;
  switch (whence)
    {
    case SEEK_SET:
      mode = FILE_BEGIN;
      break;
    case SEEK_CUR:
      mode = FILE_CURRENT;
      break;
    case SEEK_END:
      mode = FILE_END;
      break;
    }
  return (long) SetFilePointer ((HANDLE) fildes, offset, NULL, mode);
}
