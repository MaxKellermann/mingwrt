#include <ctype.h>

int
isalpha(int c)
{
  return __ISCTYPE (c, _ALPHA);
}
