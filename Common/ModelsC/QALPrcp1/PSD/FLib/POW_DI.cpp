#include "myf2c.h"

#ifdef KR_headers
double pow_di(ap, bp) double ap; long bp;
#else
double pow_di(double ap, long bp)
#endif
{
double pow, x;
long n;

pow = 1;
x = ap;
n = bp;

if(n != 0)
	{
	if(n < 0)
		{
		n = -n;
		x = 1/x;
		}
	for( ; ; )
		{
		if(n & 01)
			pow *= x;
		if(n >>= 1)
			x *= x;
		else
			break;
		}
	}
return(pow);
}
