///
///     Class to implement cubic splines
///
///     After Sedgewick, "Algorithms in C++", Addison-Wesley 1992.
///
///     Modified to incorporate linear extrapolation at the extremities
///     as described by Whiten in the JKMRC monograph:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         Appendix A
///
///
#include "stdafx.h"
#include "Spline.h"

CSpline::CSpline(void)
{
}

CSpline::~CSpline(void)
{
}

///
void CSpline::Make(double* xVals, double* yVals, int nVals)
{
    int i ;
    
    N = nVals ;
    
    if ( xVals[0] < xVals[N-1] )
	{
        for ( i = 0 ; i < N ; i++ )
		{
            X[i] = xVals[i] ;
            Y[i] = yVals[i] ;
		}
	}
	else
	{
       for ( i = 0 ; i < N ; i++ )         // invert if necessary to ensure that X is incrementing
	   {
            X[i] = xVals[N - i + 1] ;
            Y[i] = yVals[N - i + 1] ;
	   }
	}

	for ( i = 0 ; i < N ; i++ )
	{
		d[i] = 0.0 ;
		u[i] = 0.0 ;
		w[i] = 0.0 ;
		p[i] = 0.0 ;
	}

    for ( i = 1 ; i < (N - 1) ; i++ )
	{
        d[i] = 2.0 * (X[i + 1] - X[i - 1]) ;
	}

	for ( i = 0 ; i < (N-1) ; i++)
	{
        u[i] = X[i + 1] - X[i] ;
	}

	for ( i = 1 ; i < (N-1) ; i++)
	{
        w[i] = 6.0 * ((Y[i + 1] - Y[i]) / u[i] - (Y[i] - Y[i - 1]) / u[i - 1]) ;
	}
   
	for ( i = 1 ; i < (N-1) ; i++)
	{
		w[i + 1] = w[i + 1] - (w[i] * u[i] / d[i]) ;
        d[i + 1] = d[i + 1] - (u[i] * u[i] / d[i]) ;
	}
    
	for ( i = (N-2) ; i > 0 ; i--)
	{
        p[i] = (w[i] - u[i] * p[i + 1]) / d[i] ;
	}
};
///
///
///
double CSpline::Evaluate(double v)
{
    int i ;
    double  t ; 
    double t1 ; 
    double  z ; 
    double  s ; 
    double  C ; ///

	if ( v < X[0] )
	{
        z = (Y[1] - Y[0]) / (X[1] - X[0]) ;
        t = 0.0 ;
        t1 = 1 - t ;
        s = z + (X[1] - X[0]) * ((3.0 * t * t - 1) * p[1] + (3.0 * t1 * t1 - 1) * p[0]) / 6.0 ;
        C = Y[0] - s * X[0] ;
        return s * v + C ;
	}
	else if ( v > X[N - 1] )
	{
        z = (Y[N - 1] - Y[N - 2]) / (X[N - 1] - X[N - 2]) ;
        t = 0.0 ;
        t1 = 1 - t ;
        s = z + (X[N - 1] - X[N - 2]) * ((3.0 * t * t - 1) * p[N - 1] + (3.0 * t1 * t1 - 1) * p[N - 2]) / 6.0 ;
        C = Y[N - 1] - s * X[N - 1] ;
        return s * v + C ;
	}
	else
	{
        i = 0 ;
        while (v > X[i + 1])
		{
            i = i + 1 ;
		}
        
        t = (v - X[i]) / u[i] ;
        t1 = 1 - t ;

        return t * Y[i + 1] + t1 * Y[i] + u[i] * u[i] * ((t * t * t - t) * p[i + 1] + (t1 * t1 * t1 - t1) * p[i]) / 6.0 ;
	}
} ;

