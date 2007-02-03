#include <ctype.h>

int
isupper(int c)
{
  return __ISCTYPE (c, _UPPER);
}
