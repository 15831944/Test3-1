#include "stdio.h"
#include "myf2c.h"

#ifdef KR_headers
extern void f_exit();
VOID s_stop(s, n) char *s; int n;
#else
#undef abs
#include "stdlib.h"
#ifdef __cplusplus
//extern "C" {
#endif
void f_exit(void);

int s_stop(char *s, int n)
#endif
{
int i;

if(n > 0)
	{
	fprintf(stderr, "STOP ");
	for(i = 0; i<n ; ++i)
		putc(*s++, stderr);
	fprintf(stderr, " statement executed\n");
	}
//KGAFIX
//exit(0);
#ifdef __cplusplus
return 0; /* NOT REACHED */
//}
#endif
}
