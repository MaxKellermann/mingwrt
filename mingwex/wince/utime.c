#include <sys/utime.h>
#include <fcntl.h>

int 
utime (const char *path, struct utimbuf *times)
{
  int retval;
  int fildes = open (path, _O_RDWR);
  if (fildes == -1)
    return -1;

  retval = _futime (fildes, (struct _utimbuf*)times);

  close (fildes);
  return retval;
}

int
_utime (const char *fname, struct _utimbuf *times)
{
  return utime (fname, (struct utimbuf *)times);
}
