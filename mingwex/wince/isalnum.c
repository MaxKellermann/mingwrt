#include <ctype.h>

int
isalnum(int c) 
{
  return __ISCTYPE (c, (_ALPHA|_DIGIT));
}
