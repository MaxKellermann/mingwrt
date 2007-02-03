#include <ctype.h>

int
ispunct(int c)
{
  return __ISCTYPE (c, _PUNCT);
}
