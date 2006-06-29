//-- CubicSpline.h: interface for the Cubic Spline functions / classes ------

#if !defined( CUBICSPLINE_H__NEED_A_GUID )
#define       CUBICSPLINE_H__NEED_A_GUID

#include "TSMath.h"
#include "Vector.h"
#include "Matrix.h"

// declare in RioTintoTS namespace
namespace RioTintoTS
{

	// Precalculate spline parameters (derivitives)
	void SPN3( 
        int N, VectorView X, VectorView Y, VectorView& S );

	// Use precalculated spline parameters to interpolate a value of X
	double SPNV3(
        int N, VectorView X, VectorView Y, VectorView& S, double XX );

    class CubicSpline
    {

    public:

        // construct empty CubicSpline
        CubicSpline( );

        // free resources
        ~CubicSpline( );

        // set-up the spline for a given set of points
        bool SetSpline( VectorView& X, VectorView& Y,
            bool logX = false, bool logY = false );

        // calculate value at Y for a given X
        double CalculateY( const double& X );

        // test that the spline is valid for use
        bool isValid( )
        {
            return ( N_ > 0 );
        }

    private:

        void    SetEmpty( );        // set state to invalid

        bool    ascend_;            // true if values are ascending
        bool    logX_;              // true if log of X values taken
        bool    logY_;              // true if log of Y values taken
        int     N_;                 // number of points on spline
        Vector  X_;                 // x coordinates
        Vector  Y_;                 // y coordinates
        Vector  Y2_;                // derivatives
        double  u[100];             // temp values

    };

}      // namespace RioTintoTS

#endif // CUBICSPLINE_H__NEED_A_GUID
