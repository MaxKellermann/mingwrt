#include <ctype.h>

int
islower(int c)
{
  return __ISCTYPE (c, _LOWER);
}
