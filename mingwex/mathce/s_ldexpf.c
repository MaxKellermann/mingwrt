/* s_ldexpf.c -- float version of s_ldexp.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$NetBSD: s_ldexpf.c,v 1.3 1995/05/10 20:47:42 jtc Exp $";
#endif

#include "math.h"
#include "math_private.h"
#ifndef __MINGW32CE__
#include <errno.h>
#endif

float
ldexpf(float value, int exp)
{
#ifndef __MINGW32CE__
	if(!finitef(value)||value==(float)0.0) return value;
#else
	if(!finite((double)value)||value==(float)0.0) return value;
#endif
	value = scalbnf(value,exp);
#ifndef __MINGW32CE__
	if(!finitef(value)||value==(float)0.0) errno = ERANGE;
#endif
	return value;
}
