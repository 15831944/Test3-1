//-- CubicSpline2.cpp --------------------------------------------------------

#include <limits>
#include "TSMath.h"
#include "CubicSpline2.h"

using namespace RioTintoTS;


CubicSpline2::CubicSpline2( )
{

}

CubicSpline2::~CubicSpline2( )
{
    /* void */
}


bool CubicSpline2::SetSpline( VectorView& xVals, VectorView& yVals )

{
    int i;
    N = xVals.size();

	if (N <= 0) return(false);

	X.resize(N);
	Y.resize(N);
	d.resize(N);
	u.resize(N);
	P.resize(N);
	w.resize(N);

    if (xVals[0] < xVals[N-1])
	{
        for (i = 0; i < N; i++ )
		{
            X[i] = xVals[i];
            Y[i] = yVals[i];
		}
	}
	else
	{
       for (i = 0; i< N; i++) // invert if necessary to ensure that X is incrementing
	   {
            X[i] = xVals[N - 1 - i];
            Y[i] = yVals[N - 1 - i];
	   }
	}
    
    for (i = 1 ; i <  (N - 1) ; i++ )
        d[i] = 2 * (X[i + 1] - X[i - 1]);
 
    for (i = 0; i < (N - 1) ; i++ )
        u[i] = X[i + 1] - X[i];
 
    for (i = 1; i < (N - 1); i++ )
        w[i] = 6 * ((Y[i + 1] - Y[i]) / u[i] - (Y[i] - Y[i - 1]) / u[i - 1]);

    
    P[0] = 0;
    P[N-1] = 0;
    
    for (i = 1; i < (N - 1) ; i++ )
	{
        w[i + 1] = w[i + 1] - (w[i] * u[i] / d[i]);
        d[i + 1] = d[i + 1] - (u[i] * u[i] / d[i]);
	}
  
    for ( i = N - 1; i >= 2 ; i-- )
        P[i-1] = (w[i-1] - u[i-1] * P[i + 1 - 1]) / d[i-1];

	return(true);

}

double CubicSpline2::CalculateY( const double& V )

{
    int i;
    double t;
    double t1;
    double z;
    double S;
    double C;
	double Evaluate;
    
        if ( V < X[0] )
		{
            z = (Y[1] - Y[0]) / (X[1] - X[0]);
            t = 0;
            t1 = 1 - t;
            S = z + (X[1] - X[0]) * ((3 * t * t - 1) * P[1] + (3 * t1 * t1 - 1) * P[0]) / 6.0;
            C = Y[0] - S * X[0];
            Evaluate = S * V + C;
		}
		else if (V > X[N-1])
		{
            z = (Y[N-1] - Y[N - 2]) / (X[N-1] - X[N - 2]);
            t = 0;
            t1 = 1 - t;
            S = z + (X[N-1] - X[N - 2]) * ((3 * t * t - 1) * P[N-1] + (3 * t1 * t1 - 1) * P[N - 2]) / 6.0;
            C = Y[N-1] - S * X[N-1];
            Evaluate = S * V + C;
		}
		else
		{
            i = 0;
            while(V > X[i + 1])
                i = i + 1;
            
            t = (V - X[i]) / u[i];
            t1 = 1 - t;
    
            Evaluate = t * Y[i + 1] + t1 * Y[i] + 
                        u[i] * u[i] * ((t * t * t - t) * P[i + 1] + (t1 * t1 * t1 - t1) * P[i]) / 6.0;
		}

		return Evaluate;
}

