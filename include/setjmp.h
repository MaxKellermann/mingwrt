/* 
 * setjmp.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Declarations supporting setjmp and longjump, a method for avoiding
 * the normal function call return sequence. (Bleah!)
 *
 */

#ifndef _SETJMP_H_
#define _SETJMP_H_

/* All the headers include this file. */
#include <_mingw.h>

#ifndef RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The buffer used by setjmp to store the information used by longjmp
 * to perform it's evil goto-like work. The size of this buffer was
 * determined through experimentation; it's contents are a mystery.
 * NOTE: This was determined on an i386 (actually a Pentium). The
 *       contents could be different on an Alpha or something else.
 */
#if defined (__i386__)
# define _JBLEN 16
#elif defined (__arm__)
# define _JBLEN 32 /* Safe value for now. */
# warning "_JBLEN: test value with performing several setjmps/logjmp, and checking for clobber."
#endif

#define _JBTYPE int
typedef _JBTYPE jmp_buf[_JBLEN];

#ifdef __COREDLL__
_CRTIMP int __cdecl setjmp (jmp_buf);
#else
/*
 * The function provided by CRTDLL which appears to do the actual work
 * of setjmp.
 */
_CRTIMP int __cdecl _setjmp (jmp_buf);

#define	setjmp(x)	_setjmp(x)
#endif

/*
 * Return to the last setjmp call and act as if setjmp had returned
 * nVal (which had better be non-zero!).
 */
_CRTIMP void __cdecl longjmp (jmp_buf, int) __MINGW_ATTRIB_NORETURN;

#ifdef __cplusplus
}
#endif

#endif	/* Not RC_INVOKED */

#endif	/* Not _SETJMP_H_ */

