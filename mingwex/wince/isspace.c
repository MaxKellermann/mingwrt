#include <ctype.h>

int
isspace(int c)
{
  return __ISCTYPE (c, _SPACE);
}
