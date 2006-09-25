#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <_mingw.h>

static wchar_t *
mb_wcsdup (const char *str)
{
  size_t l = strlen (str) + 1;
  wchar_t *m = malloc (l * sizeof (wchar_t));
  mbstowcs (m, str, l);
  return m;
}

#define LIT_LEN(STR) \
	( (sizeof (STR "") - 1) / sizeof (STR[0]) )

_CRTIMP void __cdecl
_assert (const char *mesg, const char *file, int line)
{
  wchar_t *lmesg = mb_wcsdup (mesg);
  wchar_t *lfile = mb_wcsdup (file);

#define FORMAT L"Assertion failed: %s, at %s : %d"

  /* Leaving space for big line numbers.  */
  size_t len = wcslen (lmesg) + wcslen (lfile) + LIT_LEN (FORMAT) + 8 + 1;
  wchar_t *m = malloc (len * sizeof (wchar_t));
  wsprintfW (m, FORMAT, lmesg, lfile, line);
  OutputDebugStringW (m);
  MessageBoxW (0, L"Failed assertion", m, 0);
  free (lmesg);
  free (lfile);
  free (m);
  abort ();
}
