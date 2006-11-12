#include <windows.h>
#include <unistd.h>

int
_close (int fildes)
{
  if (CloseHandle ((HANDLE) fildes))
    return 0;
  return -1;
}
