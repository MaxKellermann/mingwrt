/*
 * dllcrt1.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Initialization code for DLLs.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <windows.h>

typedef void (* p_atexit_fn )(void);
static p_atexit_fn* first_atexit;
static p_atexit_fn* next_atexit;

/* This  is based on the function in the Wine project's exit.c */
static p_atexit_fn __dllonexit (p_atexit_fn, p_atexit_fn**, p_atexit_fn**);

//#define DEBUG

int
atexit (p_atexit_fn pfn)
{
#ifdef DEBUG
  printf ("%s: registering exit function  0x%x at 0x%x\n",
	  __FUNCTION__, (unsigned)pfn, (unsigned)next_atexit);
#endif
  return (__dllonexit (pfn,  &first_atexit, &next_atexit)
	  == NULL ? -1  : 0 );
}

_onexit_t
_onexit (_onexit_t pfn)
{
#ifdef DEBUG
  printf ("%s: registering exit function  0x%x at 0x%x\n",
	  __FUNCTION__, (unsigned)pfn, (unsigned)next_atexit);
#endif
  return ((_onexit_t) __dllonexit ((p_atexit_fn)pfn,  &first_atexit, &next_atexit));
}

static void
proc_atexit_chain (void)
{
    size_t len = next_atexit - first_atexit;

    p_atexit_fn* pfn = next_atexit;
    if (len != 0)
    {
        do 
        {
		  --pfn;
		  (*pfn)();
        } 
        while (pfn != first_atexit);
    }
}

static void
closeall_streams (void)
{
    /* Closes all except stdin/stdout/stderr.  */
    _fcloseall ();
#if 0
    fclose (stdin);
    fclose (stdout); /* closing stdout hangs the process? */
    fclose (stderr);
#endif
}

void
_cexit (void)
{
	proc_atexit_chain ();
	closeall_streams ();
}

void
exit (int code)
{
	_cexit ();
	ExitProcess (code);
}

void
_c_exit (void)
{
	closeall_streams ();
}

void
_exit (int code)
{
	closeall_streams ();
	ExitProcess (code);
}

#include "__dllonexit.c"
