// Basis Polynomials and interpolation


inline double b30(double x) 
{ 
return -(x-2)*(x-1)*x/6.;
}

inline double b31(double x) 
{ 
  return (x-2)*(x-1)*(x+1)/2.;
}

inline double  b32(double x)
{  
  return -(x-2)*x*(x+1)/2.;
}


inline double  b33(double x)
{    return (x-1)*x*(x+1)/6.;
}


double poly4(double x, double *lis);
double poly3a(double x, double *lis);
double poly3b(double x, double *lis);

// General polynomial interpolation on list:
// list values are equally spaced data points on unit interval
// len(lis) >= 4
// poly(0., lis) = lis[0]
// poly(1., lis) = lis[-1]

double polyn(double x, double *lis, int n);
double linn(double x, double *lis, int n); 
double poly3(double x, double *lis);



