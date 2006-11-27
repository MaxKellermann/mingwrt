/*
 * crt1_ce.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Source code for the startup proceedures used by all programs on a
 * wince system. This code is compiled to make crt1.o, which should be
 * located in the library path.
 *
 */

/* Hide the declaration of _fmode with dllimport attribute in stdlib.h to
   avoid problems with older GCC. */
#define __IN_MINGW_RUNTIME 
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <float.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern void __gccmain ();
extern void _pei386_runtime_relocator (void);

/* No atexit on coredll, we must initialize our private version.  */
BOOL __atexit_init(void);

/*
 * This function is called from the entry point for all programs.
 */
void
WinMainCRTStartup (HINSTANCE hInst, HINSTANCE hPrevInst,
                   LPWSTR lpCmdLine, int nCmdShow)
{
  int nRet;

  /*
   * Initialize floating point unit.
   */
  _fpreset ();			/* Supplied by the runtime library. */

   /* Adust references to dllimported data that have non-zero offsets.  */
  _pei386_runtime_relocator ();

  /*
   * Initialize the atexit table.
   */
  __atexit_init();

  /* From libgcc.a, __main calls global class constructors,
     __do_global_ctors, which registers __do_global_dtors as the first
     entry of the private atexit table we have just initialised  */
  __gccmain();

  /*
   * Call the main function. If the user does not supply one the one
   * in the 'libmingw32.a' library will be linked in, and that one
   * calls main. See winmain_ce.c in the 'lib' dir for more details.
   */

  nRet = WinMain(hInst, hPrevInst, lpCmdLine, nCmdShow);

  /*
   * Perform exit processing for the C library. This means
   * flushing output and calling 'atexit' registered functions.
   */
  _cexit ();

  ExitProcess (nRet);
}
