#include <windows.h>
#include <unistd.h>

int
_read (int fildes, void *buf, unsigned int bufsize)
{
  DWORD NumberOfBytesRead;
  if (bufsize > 0x7fffffff)
    bufsize = 0x7fffffff;
  if (!ReadFile ((HANDLE) fildes, buf, bufsize, &NumberOfBytesRead, NULL))
    return -1;
  return (int) NumberOfBytesRead;
}
