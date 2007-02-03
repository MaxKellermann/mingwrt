#include <ctype.h>

int
isprint(int c)
{
  return __ISCTYPE (c, (_BLANK|_PUNCT|_ALPHA|_DIGIT));
}
