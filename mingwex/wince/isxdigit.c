#include <ctype.h>

int
isxdigit(int c)
{
  return __ISCTYPE (c, _HEX);
}
