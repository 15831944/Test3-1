//-- CubicSpline.h: interface for the Cubic Spline functions / classes ------
/*
'''
'''     Class to implement cubic splines
'''
'''     After Sedgewick, "Algorithms in C++", Addison-Wesley 1992.
'''
'''     Modified to incorporate linear extrapolation at the extremities
'''     as described by Whiten in the JKMRC monograph:
'''         Napier-Munn et al.
'''         "Mineral Comminution Circuits - Their Operation and Optimisation",
'''         Appendix A
'''
'''
*/
#if !defined( CUBICSPLINE2_H__NEED_A_GUID )
#define       CUBICSPLINE2_H__NEED_A_GUID

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

    class CubicSpline2
    {

    public:

        // construct empty CubicSpline
        CubicSpline2( );

        // free resources
        ~CubicSpline2( );

        // set-up the spline for a given set of points
        bool SetSpline( VectorView& xVals, VectorView& yVals );

        // calculate value at Y for a given X
        double CalculateY( const double& X );

        // test that the spline is valid for use
        bool isValid( )
        {
            return ( N > 0 );
        }

    private:

		int N;
		Vector X;
		Vector Y;
		Vector d;
		Vector u;
		Vector P;
		Vector w;

    };

}      // namespace RioTintoTS

#endif // CUBICSPLINE2_H__NEED_A_GUID