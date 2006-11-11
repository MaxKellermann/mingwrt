#include <windows.h>
#include <unistd.h>

int
write (int fildes, const void *buf, unsigned int bufsize)
{
  DWORD NumberOfBytesWritten;
  if (bufsize > 0x7fffffff)
    bufsize = 0x7fffffff;
  if (!WriteFile ((HANDLE) fildes, buf, bufsize, &NumberOfBytesWritten, NULL))
    return -1;
  return (int) NumberOfBytesWritten;
}
