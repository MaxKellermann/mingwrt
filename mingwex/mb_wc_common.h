#ifndef __COREDLL__
#include <locale.h>
#endif
#include <string.h>
#include <stdlib.h>

static inline
unsigned int get_cp_from_locale (void)
{
  char* cp_string;
  /*
    locale :: "lang[_country[.code_page]]" 
               | ".code_page"

  */

#ifndef __COREDLL__
  if ((cp_string = strchr(setlocale(LC_CTYPE, NULL), '.')))
    return  ((unsigned) atoi (cp_string + 1));
#endif
  return 0;
}
