//-- CubicSpline.cpp --------------------------------------------------------

#include <limits>
#include "TSMath.h"
#include "CubicSpline.h"

using namespace RioTintoTS;


CubicSpline::CubicSpline( )
{
    SetEmpty( );
}

CubicSpline::~CubicSpline( )
{
    /* void */
}

bool CubicSpline::SetSpline( VectorView& X, VectorView& Y, bool logX, bool logY )
{
    int i,k;
    double p,sig;

    // determine element count
    N_ = X.size();

    // test that X and Y same size
    if( N_ < 2 || N_ > 100 ||  N_ != Y.size() )
        goto initFailed;

    // dimension structures for N elements
    X_.resize( N_ );
    Y_.resize( N_ );
    Y2_.resize( N_ );

    // put values into local X_ and Y_ vectors

    if( X[N_-1] < X[0] )
    {
        // Case 1: X_ values descending
        X_ = X.reverse();
        Y_ = Y.reverse();
    }
    else
    {
        // Caes 2: X_ values ascending
        X_ = X;
        Y_ = Y;
    }

    // convert X axis to its log if required
    
    if( logX )
    {
        logX_ = true;
        if( !X_.setLog() )
            goto initFailed;
    }

    // convert Y axis to its log if required

    if( logY )
    {
        logY_ = true;
        if( !Y_.setLog() )
            goto initFailed;
    }

    // use natural spline at boundaries

    Y2_[0]    = u[0]    = 0;
    Y2_[N_-1] = u[N_-1] = 0;

    // decomposition loop - tri-diagonal algorithm

    for( i=1; i<(N_-1); i++ )
    {
        sig    = ( X_[i] - X_[i-1] ) / ( X_[i+1] - X_[i-1] );

        p      = sig * Y2_[i-1] + 2.0;

        Y2_[i] = ( sig - 1.0 ) / p;

        u[i]   = ( Y_[i+1] - Y_[i] ) / ( X_[i+1] - X_[i] )
               - ( Y_[i] - Y_[i-1] ) / ( X_[i] - X_[i-1] );

        u[i]   = ( ( 6.0 * u[i] ) / ( X_[i+1] - X_[i-1] ) - sig * u[i-1] ) / p;
    }

    // backsubstitition loop - tri-diagonal algorithm

    for( k = N_-2; k >= 0; k -- )
    {
        Y2_[k] = Y2_[k] * Y2_[k+1] + u[k];
    }

    // SetSpline succeeded
    return true;

initFailed:

    // Set empty state
    SetEmpty( );

    // SetSpline failed
    return false;
}

void CubicSpline::SetEmpty( )
{
    N_ = 0;
    X_.resize( 0 );
    Y_.resize( 0 );
    Y2_.resize( 0 );
}

double CubicSpline::CalculateY( const double& X )
{
    int klo, khi, k;
    double h, b, a;

    double Y;

    if( N_ == 0 )
    {
        return std::numeric_limits<double>::quiet_NaN();
        return 0;
    }

    klo = 0;
    khi = N_ - 1;
    
    while( khi-klo > 1 )
    {
        k = (khi + klo ) >> 1;
        if( X_[k] > X )
            khi = k;
        else
            klo = k;
    }

    h = X_[khi] - X_[klo];

    if( fabs(h) < 1e-10 )
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    a = ( X_[khi] - X ) / h;
    b = ( X - X_[klo] ) / h;

    Y = a * Y_[klo] + b * Y_[khi]
      + ( (a*a*a - a) * Y2_[klo] + (b*b*b - b) * Y2_[khi] ) * (h * h)/6.0;

    return Y;
}

//-- Precalculate cubic spline parameters -----------------------------------

void RioTintoTS::SPN3( int N, VectorView X, VectorView Y, VectorView& S )
{
	double DI;
	double AI;
	double AI1;
	double CI;
	double PI;
	double PI1;
	double Z;

	int	   i;
	int    j;
	int    NM1;

	static double C[100];

	if( N>2 && N<100 )
	{
        // Reverse vectors if X descending order
        if( X[N-1] < X[0] )
        {
            X.reset( X.reverse() );
            Y.reset( Y.reverse() );
        }

		// End points

		NM1    = N-1;
		S[0]   = 0;
		S[NM1] = 0;

		// Initial values

		DI = 0.0;
		AI = X[1] - X[0];
		CI = -AI * S[0];
		PI = ( Y[1] - Y[0] ) / ( AI + 1e-30 );

		// Form equations and reduce to triangular form

		for( i=1; i<NM1; i++ )
		{
			// Segment width
			AI1 = X[i+1] - X[i];
			
			// Diagonal elements
			Z = 2.0 * (AI1+AI) - DI;

			// Constant term
			PI1  = ( Y[i+1] - Y[i] ) / ( AI1 + 1e-30 );
			C[i] = ( 6.0*(PI1-PI) - CI ) / ( Z + 1e-30 );
			CI   = C[i] * AI1;

			// Upper daigonal
			S[i] = AI1 / ( Z + 1e-30 );
			DI   = S[i] * AI1;
			AI   = AI1;
		}

		// Back substitute

		for( j=(NM1-1); j>0; j-- )
		{
			S[j] = C[j] - S[j]*S[j+1];
		}
	}
}




//-- Use pre-calculated spline parameters to get value of spline at X -------

double RioTintoTS::SPNV3
(
    int         N,
    VectorView  X,
    VectorView  Y,
    VectorView& S,
    double      XX
)
{
	double XIXI1 = 0.0;
	double DX    = 0.0;
	double XXI   = 0.0;
	double YY    = 0.0;
	double T     = 0.0;

    // Reverse vectors if X descending order

    if( X[N-1] < X[0] )
    {
        X.reset( X.reverse() );
        Y.reset( Y.reverse() );
    }

	// Find interval containing XX

	if( XX < X[0] )
	{
		// before X[0]

		XIXI1 = X[0] - X[1];
		DX    = (Y[0]-Y[1])/(XIXI1+1e-30) + XIXI1*(S[1]+2*S[0])/6.0;
		XXI   = XX - X[0];

		YY = Y[0] + XXI * ( DX + XXI*S[0] );
	}
	else if( XX > X[N-1] )
	{
		// after X[N-1]

		XIXI1 = X[N-1] - X[N-2];
		DX    = (Y[N-1]-Y[N-2])/(XIXI1+1e-30) + XIXI1*(2*S[N-1]+S[N-2])/6.0;
		XXI	 = XX - X[N-1];

		YY = Y[N-1] + XXI * ( DX + XXI*S[N-1] );
	}
	else
	{
		int i     = 0;
		int left  = 0;
		int right = N-1;
		int mid   = 0;

		while( XX>=X[left] && XX<=X[right] )
		{
			mid = left + (right-left)/2;

			if( mid==left )
			{
				i = left;
				break;
			}
			else if( XX < X[mid] )
			{
				right = mid;
			}
			else
			{
				left = mid;
			}
		}

		// between X[i] and X[i+1]

        double x1 = X[i];
        double x2 = X[i+1];
        double y1 = Y[i];
        double y2 = Y[i+1];


		XXI   = XX   - X[i];
		XIXI1 = X[i] - X[i+1];
		T     = 2.0*S[i] + XXI*(S[i]-S[i+1])/(XIXI1+1e-30) + S[i+1];

		YY = Y[i] + XXI*(Y[i]-Y[i+1])/(XIXI1+1e-30) + (XX-X[i+1])*T/6.0;
	}

	return YY;
}


//-- CubicSpline.cpp --------------------------------------------------------
