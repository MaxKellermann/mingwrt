/*  ctanh.c */

/*
   Contributed by Danny Smith
   2003-10-20
*/


#include <math.h>
#include <complex.h>
#ifndef UNDER_CE
#include <errno.h>
#endif

/*
  ctanh (x + I * y) = (sinh (2 * x)  +  sin (2 * y) * I )
		     / (cosh (2 * x) + cos (2 * y)) .
*/

double complex
ctanh (double complex Z)
{
  double complex Res;
  double two_R = 2.0 * __real__ Z;
  double two_I = 2.0 * __imag__ Z;
  double denom = cosh (two_R) + cos (two_I);

  if (denom == 0.0)
    {
#ifndef UNDER_CE
      errno = ERANGE;
#endif
      __real__ Res = HUGE_VAL;
      __imag__ Res = HUGE_VAL;
    }
  else if ( isinf (denom))
    {
#ifndef UNDER_CE
      errno = ERANGE;
#endif
      __real__ Res = two_R > 0 ? 1.0 : -1.0;
      __imag__ Res = 0.0;
    }
  else
    {
      __real__ Res = sinh (two_R) / denom;
      __imag__ Res = sin (two_I) / denom;
    }
  return Res;
}
