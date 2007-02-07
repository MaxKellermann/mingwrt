/*  MB_CUR_MAX implementation for Windows CE.
 *  Revision history:
 *  6 Feb 2007	Initial version.
 *
 *  Contributor:   Pedro Alves <pedro_alves@portugalmail.pt>
 */

 /* This routine has been placed in the public domain. */

#include <windows.h>

int
__mb_cur_max (void)
{
  CPINFO cpinfo;
  GetCPInfo (CP_ACP, &cpinfo);
  return cpinfo.MaxCharSize;
}
