/* EFL support routine to compare two character strings */

#include "f2c.h"

#ifdef KR_headers
extern integer s_cmp();
integer G77_ef1cmc_0 (a, la, b, lb) ftnint *a, *b; ftnlen *la, *lb;
#else
extern integer s_cmp(char*,char*,ftnlen,ftnlen);
integer G77_ef1cmc_0 (ftnint *a, ftnlen *la, ftnint *b, ftnlen *lb)
#endif
{
return( s_cmp( (char *)a, (char *)b, *la, *lb) );
}
