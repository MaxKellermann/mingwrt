/*
 * atexit.c
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

//#define DEBUG

#ifdef DEBUG
# define TRACE(FMT, ...) \
	printf ("trace: %s:%d : " FMT, __FILE__, __LINE__, ##__VA_ARGS__)

# define FIXME(FMT, ...) \
	printf ("fixme: %s:%d : " FMT, __FILE__, __LINE__, ##__VA_ARGS__)
#else
# define TRACE(FMT, ...) do; while (0)
# define FIXME(FMT, ...) do; while (0)
#endif

typedef void (* p_atexit_fn )(void);
static p_atexit_fn* first_atexit;
static p_atexit_fn* next_atexit;

/* This  is based on the function in the Wine project's exit.c */
p_atexit_fn __dllonexit (p_atexit_fn, p_atexit_fn**, p_atexit_fn**);

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

p_atexit_fn* __atexit_first(void)
{
  return first_atexit;
}

BOOL __atexit_init(void)
{
  /* Initialize atexit table.
  	32 is min size required by ANSI */
  first_atexit = (p_atexit_fn*) malloc (32 * sizeof (p_atexit_fn));
  if (first_atexit == NULL ) /* can't allocate memory */
  {
#if defined(__MINGW32CE__) && !defined(__COREDLL__)
	errno=ENOMEM;
#endif
	return FALSE;
  }
  *first_atexit =  NULL;
  next_atexit = first_atexit;
  return TRUE;
}

void __dll_exit(void)
/* Run LIFO terminators registered in private atexit table */
{
  if ( first_atexit )
    {
      p_atexit_fn* __last = next_atexit - 1;
      while ( __last >= first_atexit )
	{	
          if ( *__last != NULL )
	    {
#ifdef DEBUG
	      printf ("%s: Calling exit function  0x%x from 0x%x\n",
		      __FUNCTION__, (unsigned)(*__last),(unsigned)__last);
#endif
              (**__last) ();
	    }
	  __last--;
	}
      free ( first_atexit ) ;
      first_atexit = NULL ;
    }
    /*
       Make sure output buffers opened by DllMain or 
       atexit-registered functions are flushed before detaching,
       otherwise we can have problems with redirected output.
     */
    fflush (NULL);
}

p_atexit_fn 
__dllonexit(p_atexit_fn func, p_atexit_fn **start, p_atexit_fn **end)
{
  p_atexit_fn *tmp;
  int len;

  TRACE("(%p,%p,%p)\n", func, start, end);

  if (!start || !*start || !end || !*end)
  {
   FIXME("bad table\n");
   return NULL;
  }

  len = (*end - *start);

  TRACE("table start %p-%p, %d entries\n", *start, *end, len);

  if (++len <= 0)
    return NULL;

  tmp = (p_atexit_fn *)realloc(*start, len * sizeof(tmp));
  if (!tmp)
    return NULL;
  *start = tmp;
  *end = tmp + len;
  tmp[len - 1] = func;
  TRACE("new table start %p-%p, %d entries\n", *start, *end, len);
  return func;
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
	__dll_exit ();
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
