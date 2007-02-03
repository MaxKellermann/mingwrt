#include <ctype.h>

int
isgraph(int c)
{
  return __ISCTYPE (c, (_PUNCT|_ALPHA|_DIGIT));
}
