/*
 * main.c
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Extra startup code for applications which do not have a main function
 * of their own (but do have a WinMain). Generally these are GUI
 * applications, but they don't *have* to be.
 *
 */

#include <stdlib.h>
#include <process.h>
#include <windows.h>

#define ISSPACE(a)	(a == ' ' || a == '\t')

#ifndef UNDER_CE
extern int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, 
                           LPSTR szCmdLine, int nShow);
#else
extern int __cdecl WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, 
                           LPWSTR szCmdLine, int nShow);

#endif

int
main (int argc, char *argv[], char *environ[])
{
  int nRet;

  /* Get the command line passed to the process. */
#ifndef UNDER_CE
  char *szCmd;
  STARTUPINFOA startinfo;
  szCmd = GetCommandLineA ();
  GetStartupInfoA (&startinfo);
#else
  wchar_t *szCmd;
  szCmd = GetCommandLineW ();
#endif

  /* Strip off the name of the application and any leading
   * whitespace. */
  if (szCmd)
    {
      while (ISSPACE (*szCmd))
	{
	  szCmd++;
	}

      /* On my system I always get the app name enclosed
       * in quotes... */
      if (*szCmd == '\"')
	{
	  do
	    {
	      szCmd++;
	    }
	  while (*szCmd != '\"' && *szCmd != '\0');

	  if (*szCmd == '\"')
	    {
	      szCmd++;
	    }
	}
      else
	{
	  /* If no quotes then assume first token is program
	   * name. */
	  while (!ISSPACE (*szCmd) && *szCmd != '\0')
	    {
	      szCmd++;
	    }
	}

      while (ISSPACE (*szCmd))
	{
	  szCmd++;
	}
    }

#ifndef UNDER_CE
  nRet = WinMain (GetModuleHandle (NULL), NULL, szCmd,
		  (startinfo.dwFlags & STARTF_USESHOWWINDOW) ?
		  startinfo.wShowWindow : SW_SHOWDEFAULT);
#else
  nRet = WinMain (GetModuleHandle (NULL), NULL, szCmd, SW_SHOW);
#endif

  return nRet;
}

