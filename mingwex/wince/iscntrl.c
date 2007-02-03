#include <ctype.h>

 int
iscntrl(int c)
{
  return __ISCTYPE (c, _CONTROL);
}
