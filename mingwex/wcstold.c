/*  Wide char wrapper for strtold
 *  Revision history:
 *  6 Nov 2002	Initial version.
 *  25 Aug 2006  Don't use strtold internal functions.
 *
 *  Contributor:   Danny Smith <dannysmith@users.sourceforege.net>
 */

 /* This routine has been placed in the public domain.*/

#ifdef __COREDLL__
/* coredll.dll doesn't export strtold.  */
#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>

#include "mb_wc_common.h"

#include <mbstring.h>

#ifdef __COREDLL__
static size_t
_mbslen(const unsigned char *str)
{
  size_t len;

  if(!str)
    return 0;

  if(MB_CUR_MAX == 1)
    return strlen((const char*)str);

  len = 0;
  for(; *str; str++)
  {
    if (IsDBCSLeadByte(*str))
	 str++;
    len++;
  }
  return len;
}
#endif

long double wcstold (const wchar_t * __restrict__ wcs, wchar_t ** __restrict__ wcse)
{
  char * cs;
  char * cse;
  unsigned int i;
  long double ret;
  const unsigned int cp = get_codepage ();

  /* Allocate enough room for (possibly) mb chars */
  cs = (char *) malloc ((wcslen(wcs)+1) * MB_CUR_MAX);

  if (cp == 0) /* C locale */
    {
      for (i = 0; (wcs[i] != 0) && wcs[i] <= 255; i++)
        cs[i] = (char) wcs[i];
      cs[i]  = '\0';
    }
  else
    {
      int nbytes = -1;
      int mb_len = 0;
      /* loop through till we hit null or invalid character */
      for (i = 0; (wcs[i] != 0) && (nbytes != 0); i++)
	{
     	  nbytes = WideCharToMultiByte(cp, WC_COMPOSITECHECK | WC_SEPCHARS,
				       wcs + i, 1, cs + mb_len, MB_CUR_MAX,
				       NULL, NULL);
	  mb_len += nbytes;
	}
      cs[mb_len] = '\0';
    }

  ret =  strtold (cs, &cse);

  if (wcse)
    {
      /* Make sure temp mbstring has 0 at cse.  */ 
      *cse = '\0';
      i = _mbslen ((unsigned char*) cs); /* Number of chars, not bytes */
      *wcse = (wchar_t *) wcs + i;
    }
  free (cs);

  return ret;
}

#endif /* __COREDLL__ */
