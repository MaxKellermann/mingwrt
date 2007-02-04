
/* Written by Pedro Alves <pedro_alves@portugalmail.pt>
   Public Domain */

#include <math.h>

float
log2f (float v)
{
   return (float)((double)logf(v) / M_LOG2E);
}
