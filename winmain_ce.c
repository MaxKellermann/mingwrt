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
int _argc = 0;
char **_argv = 0;

static int
_parse_tokens(char* string, char*** tokens, int length)
{
    /* Extract whitespace- and quotes- delimited tokens from the given string
       and put them into the tokens array. Returns number of tokens
       extracted. Length specifies the current size of tokens[].
       THIS METHOD MODIFIES string.  */

    const char* whitespace = " \t\r\n";
    char* tokenEnd;
    const char* quoteCharacters = "\"\'";
    char* end = string + strlen (string);

    if (string == NULL)
        return length;

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

        {
            char** new_tokens;
            int newlen = length + 1;
            new_tokens = realloc (*tokens, sizeof (char**) * newlen);
            if (!new_tokens)
            {
                /* Out of memory.  */
                return -1;
            }

            *tokens = new_tokens;
            (*tokens)[length] = string;
            length = newlen;
        }
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
    modlen = GetModuleFileNameW(NULL, cmdnameBufW, sizeof (cmdnameBufW)/sizeof (cmdnameBufW[0]));

    if (!cmdlinePtrW)
        cmdlineLen = 0;
    else
        cmdlineLen = wcslen(cmdlinePtrW);

    __cmdlinebuf = malloc (modlen + 1 + cmdlineLen + 1);
    if (!__cmdlinebuf)
        ExitProcess(-1);

    *argv = malloc (sizeof (char**) * 1);
    if (!*argv)
        ExitProcess(-1);

    (*argv)[0] = __cmdlinebuf;
    wcstombs((*argv)[0], cmdnameBufW, wcslen(cmdnameBufW) + 1);
    /* Add one to account for argv[0] */
    (*argc)++;

    if (cmdlineLen > 0)
    {
        char* argv1 = (*argv)[0] + strlen((*argv)[0]) + 1;
        wcstombs(argv1, cmdlinePtrW, cmdlineLen + 1);
        *argc = _parse_tokens(argv1, argv, 1);
        if (*argc < 0)
            ExitProcess(-1);
    }
    (*argv)[*argc] = 0;
    return;
}

// Normally, the application will define a WinMain function.  However,
// if the main application does not, this dummy WinMain will call a
// main() function instead.
extern int __cdecl
WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
        LPWSTR szCmdLine, int nShow)
{
  /*
   * Set up __argc, __argv.
   */
  __mainArgs(&_argc, &_argv, szCmdLine);

  return main(_argc, _argv);
}
