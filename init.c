/*
 * init.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Code to initialize standard file handles and command line arguments.
 * This file is #included in both crt1.c and dllcrt1.c.
 *
 */

/*
 * Access to a standard 'main'-like argument count and list. Also included
 * is a table of environment variables.
 */
int _argc = 0;
char **_argv = 0;

/* NOTE: Thanks to Pedro A. Aranda Gutiirrez <paag@tid.es> for pointing
 * this out to me. GetMainArgs (used below) takes a fourth argument
 * which is an int that controls the globbing of the command line. If
 * _CRT_glob is non-zero the command line will be globbed (e.g. *.*
 * expanded to be all files in the startup directory). In the mingw32
 * library a _CRT_glob variable is defined as being -1, enabling
 * this command line globbing by default. To turn it off and do all
 * command line processing yourself (and possibly escape bogons in
 * MS's globbing code) include a line in one of your source modules
 * defining _CRT_glob and setting it to zero, like this:
 *  int _CRT_glob = 0;
 */
extern int _CRT_glob;

#ifdef __MSVCRT__
typedef struct {
  int newmode;
} _startupinfo;
extern void __getmainargs (int *, char ***, char ***, int, _startupinfo *);
#elif defined (__CRTDLL__)
extern void __GetMainArgs (int *, char ***, char ***, int);
#elif defined (__COREDLL__)
static void __GetMainArgs (int *, char ***, char ***, int);
#endif

/*
 * Initialize the _argc, _argv and environ variables.
 */
static void
_mingw32_init_mainargs ()
{
  /* The environ variable is provided directly in stdlib.h through
   * a dll function call. */
  char **dummy_environ;
#ifdef __MSVCRT__
  _startupinfo start_info;
  start_info.newmode = 0;
#endif

  /*
   * Microsoft's runtime provides a function for doing just that.
   */
#if defined (__MSVCRT__)
  (void) __getmainargs (&_argc, &_argv, &dummy_environ, _CRT_glob, 
                        &start_info);
#else
  /* CRTDLL/COREDLL version */
  (void) __GetMainArgs (&_argc, &_argv, &dummy_environ, _CRT_glob);
#endif
}

#ifdef __COREDLL__

static int
_parse_tokens(char* string, char*** tokens, int start, int length)
{
    /* Extract whitespace- and quotes- delimited tokens from the given string
        and put them into the tokens array. Returns number of tokens
        extracted. Length specifies the current size of tokens[].
        THIS METHOD MODIFIES string.  */

    char*  whitespace = " \t\r\n";
    char*  tokenEnd;
    char*  quoteCharacters = "\"\'";
    int numQuotes = strlen(quoteCharacters);
    int quoteIndex;
    char quote;
    int ntokens = 0;

    if (string == NULL)
        return 0;

    while (1)
    {
        /* Skip over initial whitespace.  */
        string += strspn(string, whitespace);
        if (strlen(string) == 0)
            break;
        quoteIndex = numQuotes;
        while (--quoteIndex >= 0) 
            if (*string == quoteCharacters[quoteIndex]) 
                break;
        if (quoteIndex >= 0) 
        {
            /* Token is quoted.  */
            quote = *string++;
            tokenEnd = strchr(string, quote);
            /* If there is no endquote, the token is the rest of the string.  */
            if (!tokenEnd)
                tokenEnd = string + strlen(string);
        } 
        else 
        {
            tokenEnd = string + strcspn(string, whitespace);
        }
        if ((length-start) <= ntokens)
        {
            char*** new_tokens;
            int newlen = length;
            /* Double memory.  */
            newlen += (length == 0) ? 1 : newlen;
            new_tokens = realloc (tokens, sizeof (char**) * newlen);
            if (!new_tokens)
            {
                /* Out of memory.  */
                goto exit;
            }
            length = newlen;
            tokens = new_tokens;
        }
        (*tokens)[ntokens+start] = string;
        ntokens++;
        if (*tokenEnd == '\0') 
            break;
        *tokenEnd = '\0';
        string = tokenEnd + 1;
    }
exit:
    return ntokens;
}

static void
__GetMainArgs (int *argc, char ***argv, char *** env, int glob)
{
    wchar_t  cmdnameBufW[512];
    char     buf[MAX_PATH];
    int      cmdlineLen = 0;
    wchar_t* cmdlinePtrW;
    int modlen;
    char* __cmdlinebuf;
    int argv_len = 1;

    /* argv[0] is the path of invoked program - get this from CE.  */
    cmdnameBufW[0] = 0;
    modlen = GetModuleFileNameW(NULL, cmdnameBufW, sizeof (cmdnameBufW)/sizeof (cmdnameBufW[0]));
    cmdlinePtrW = GetCommandLineW();

    if (!cmdlinePtrW)
        cmdlineLen = 0;
    else
        cmdlineLen = wcslen(cmdlinePtrW);

    __cmdlinebuf = malloc (modlen + 1 + cmdlineLen + 1);
    if (!__cmdlinebuf)
        ExitProcess(-1);

    *argv = malloc (sizeof (char**) * argv_len);
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
        *argc += _parse_tokens(argv1, argv, 1, argv_len);
    }
    (*argv)[*argc] = 0;
    return;
}

#endif
