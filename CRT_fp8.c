/*
 * CRT_FP8.c
 *
 * This forces calls of _fpreset to the MSVCRT function
 * exported from dll.  Effectively it make default
 * precison same as apps built with MSVC (53-bit mantissa).

 *
 * To change to 64-bit mantissa, link in CRT_FP10.o before libmingw.a. 
 */

/* Link against the _fpreset visible in import lib */

#include <_mingw.h>

extern void (*__IMP(_fpreset))(void) ;
void _fpreset (void)
{  (*__IMP(_fpreset))(); }

void __attribute__ ((alias ("_fpreset"))) fpreset(void);
