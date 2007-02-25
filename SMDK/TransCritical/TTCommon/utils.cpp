#include "utils.h"

double poly4(double x, double* lis) 
{
  return b30(x)*lis[0]+b31(x)*lis[1]+b32(x)*lis[2]+b33(x)*lis[3];
}

double  poly3a(double x, double* lis)
{
  return lis[0]*x*(x-1)/2.+lis[1]*(1-x*x)+lis[2]*x*(x+1)/2.;
}


double poly3b(double x, double* lis)
{
  return lis[0]*(x-1)*(x-2)/2.+lis[1]*x*(2-x)+lis[2]*x*(x-1)/2.;
}



// General polynomial interpolation on list:
// list values are equally spaced data points on unit interval
// len(lis) >= 4
// poly(0., lis) = lis[0]
//  poly(1., lis) = lis[-1]


double polyn(double x, double * lis, int n)
{
  int l1 = n-1;
  if (x==1.0) return lis[l1];
  double x1 = x*l1;
  int i1 = (int) x1;
  x1 -= i1;
  if (i1==0)
    return poly3b(x1, lis);
  else if (i1==l1-1)
    return poly3a(x1, lis+n-3);
  else
    return poly4(x1, lis+i1-1);
}

double linn(double x, double * lis, int n)
{
  int l1 = n-1;
  if (x==1.0) return lis[l1];
  double x1 = x*l1;
  int i1 = (int) x1;
  x1 -= i1;
  return (1-x1)*lis[i1]+x1*lis[i1+1];
}


double poly3(double x, double *lis) 
{
  return (2*x-1)*(x-1)*lis[0]+4*x*(1-x)*lis[1]+x*(2*x-1)*lis[2];
}
