
/* Written by Pedro Alves <pedro_alves@portugalmail.pt>
   Public Domain */

#include <math.h>

double
log2 (double v)
{
   return log(v) / M_LOG2E;
}
