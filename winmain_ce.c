/*
 * winmain_ce.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Extra startup code for applications which do not have a WinMain
 * function of their own (but do have a main). Generally these are
 * non-GUI applications, but they don't *have* to be.
 *
 */

#include <stdlib.h>
#include <process.h>
#include <windows.h>

/*
 * Access to a standard 'main'-like argument count and list. Also included
 * is a table of environment variables.
 */
int __argc = 0;
char **__argv = 0;

extern int main (int, char **, char **);

/* Extract whitespace- and quotes- delimited tokens from the given
 * string and put them into the tokens array. Returns number of tokens
 * extracted. Length specifies the current size of tokens[], not
 * counting for the NULL terminator.  THIS METHOD MODIFIES string.
 */
static int
_parse_tokens(char* string, char*** tokens, int length)
{
    const char* whitespace = " \t\r\n";
    char* tokenEnd;
    const char* quoteCharacters = "\"\'";
    char* end;

    if (string == NULL)
        return length;

    end = string + strlen (string);

    while (1)
    {
        const char* q;
        /* Skip over initial whitespace.  */
        string += strspn(string, whitespace);
        if (*string == '\0')
            break;

        for (q = quoteCharacters; *q; ++q)
        {
            if (*string == *q)
                break;
        }
        if (*q)
        {
            /* Token is quoted.  */
            char quote = *string++;
            tokenEnd = strchr(string, quote);
            /* If there is no endquote, the token is the rest of the string.  */
            if (!tokenEnd)
                tokenEnd = end;
        }
        else
        {
            tokenEnd = string + strcspn(string, whitespace);
        }

        *tokenEnd = '\0';

	length++;
	*tokens = realloc (*tokens, sizeof (char**) * (length + 1));
	if (!*tokens)
	  {
	    /* Out of memory.  */
	    return -1;
	  }

	(*tokens)[length - 1] = string;
        if (tokenEnd == end)
            break;
        string = tokenEnd + 1;
    }
exit:
    return length;
}

static void
__mainArgs(int *argc, char ***argv, wchar_t *cmdlinePtrW)
{
    wchar_t  cmdnameBufW[512];
    char     buf[MAX_PATH];
    int      cmdlineLen = 0;
    int modlen;
    char* __cmdlinebuf;

    /* argv[0] is the path of invoked program - get this from CE.  */
    cmdnameBufW[0] = 0;
    modlen = GetModuleFileNameW(NULL, cmdnameBufW,
				sizeof (cmdnameBufW)/sizeof (cmdnameBufW[0]));

    if (!cmdlinePtrW)
        cmdlineLen = 0;
    else
        cmdlineLen = wcslen(cmdlinePtrW);

    __cmdlinebuf = malloc (modlen + 1 + cmdlineLen + 1);
    if (!__cmdlinebuf)
        ExitProcess(-1);

    /* Add one to account for argv[0] */
    (*argc)++;

    /* + 1 accounts for the null terminator.  */
    *argv = malloc (sizeof (char**) * (*argc + 1));
    if (!*argv)
        ExitProcess(-1);

    (*argv)[0] = __cmdlinebuf;
    wcstombs((*argv)[0], cmdnameBufW, wcslen(cmdnameBufW) + 1);

    if (cmdlineLen > 0)
    {
        char* argv1 = (*argv)[0] + strlen((*argv)[0]) + 1;
        wcstombs(argv1, cmdlinePtrW, cmdlineLen + 1);
        *argc = _parse_tokens(argv1, argv, 1);
        if (*argc < 0)
            ExitProcess(-1);
    }
    (*argv)[*argc] = NULL;
    return;
}

/* Normally, the application will define a WinMain function.  However,
 * if the main application does not, this dummy WinMain will call a
 * main() function instead.
 */
int
WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
	 LPWSTR szCmdLine, int nShow)
{
  /*
   * Set up __argc, __argv.
   */
  __mainArgs(&__argc, &__argv, szCmdLine);

  /* Pass in a NULL environ for those apps that expect it.  Better
     than stack garbage.  */
  return main(__argc, __argv, NULL);
}
