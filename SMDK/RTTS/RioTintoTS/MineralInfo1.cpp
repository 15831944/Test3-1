//-- MineralInfo1.cpp -------------------------------------------------------

#include <memory.h>
#include "MineralInfo1.h"
#include "CubicSpline.h"

using namespace RioTintoTS;
#pragma optimize("", off)


/****************************************************************************
*
*   MineralInfo1:: class constants
*
*   Default values for unsupplied MineralInfo parameters
*
****************************************************************************/
const double MineralInfo1::DEFAULT_SG   = 2.0;
const double MineralInfo1::DEFAULT_A    = 60.0;
const double MineralInfo1::DEFAULT_B    = 2.0;
const double MineralInfo1::DEFAULT_TA   = 0.8;
const double MineralInfo1::DEFAULT_M    = 0.0677;
const double MineralInfo1::DEFAULT_C    = 1.2511;


/****************************************************************************
*
*   MineralInfo1::MineralInfo1( )
*
*   Construct default MineralInfo1 object
*
****************************************************************************/
MineralInfo1::MineralInfo1( )
{
    // Dimension appearance spline data 

    PARAMT.resize( 7 );

    X.resize( 4 );

    Y2.resize( 4 );
    Y4.resize( 4 );
    Y25.resize( 4 );
    Y50.resize( 4 );
    Y75.resize( 4 );

    S2.resize( 4 );
    S4.resize( 4 );
    S25.resize( 4 );
    S50.resize( 4 );
    S75.resize( 4 );

    XX.resize( 7 );
    YY.resize( 7 );
    SS.resize( 7 );
}


/****************************************************************************
*
*   MineralInfo1::SetMineralInfo( )
*
*   Sets mineral property values
*
****************************************************************************/
bool MineralInfo1::SetMineralInfo
(
    double SG, double A, double b, double ta, double m, double c
)
{
    // Set members
    SG_ = SG;
    A_  = A;
    b_  = b;
    ta_ = ta;
    m_  = m;
    c_  = c;

    // Always succeed ?
    return true;
}


/****************************************************************************
*
*   MineralInfo1::SetAppearance( )
*
*   Sets mineral breakage appearance matrix
*
****************************************************************************/
bool MineralInfo1::SetAppearance
(
    const VectorView& t10Values,
    const VectorView& tValues,
    const MatrixView& appMatrix
)
{
    // Set appearance matrix
    appMatrix_ = appMatrix;

    // Pre-calculate cubic splines
    AppearancePreCalc( );

    // Always succeed ?
    return true;
}
    
    
/****************************************************************************
*
*	MineralInfo1::AppearancePreCalc( )
*
*	Pre-calculate splines for a new appearance function
*
****************************************************************************/
void MineralInfo1::AppearancePreCalc( )
{
    // Refer to rows of the appearance matrix
    VectorView R1 = appMatrix_[0];
    VectorView R2 = appMatrix_[1];
    VectorView R3 = appMatrix_[2];
    
    // Setup spline vectors

	X[0] =  0.0;
	X[1] = 10.0;
	X[2] = 20.0;
	X[3] = 30.0;

	Y2[0] =  0.00;
	Y2[1] = R1[4];
	Y2[2] = R2[4];
	Y2[3] = R3[4];

	Y4[0] =  0.00;
	Y4[1] = R1[3];
	Y4[2] = R2[3];
	Y4[3] = R3[3];

	Y25[0] =  0.00;
	Y25[1] = R1[2];
	Y25[2] = R2[2];
	Y25[3] = R3[2];

	Y50[0] =  0.00;
	Y50[1] = R1[1];
	Y50[2] = R2[1];
	Y50[3] = R3[1];

	Y75[0] =  0.00;
	Y75[1] = R1[0];
	Y75[2] = R2[0];
	Y75[3] = R3[0];

    // Calculate splines

    SPN3( 4, X,  Y2,  S2 );
    SPN3( 4, X,  Y4,  S4 );
    SPN3( 4, X, Y25, S25 );
    SPN3( 4, X, Y50, S50 );
    SPN3( 4, X, Y75, S75 );
}


/****************************************************************************
*
*   MineralInfo1::BreakRockRR( )
*
*   Break a quantity of rock into a size distribution of daughter particles
*   depending on the strength of a applied reduction ratio to the rock.
*
****************************************************************************/
double MineralInfo1::BreakRockRR
(
    double&         RockSize,       // The size of the rocks to break
    double&         Quantity,       // The mass/volume of the rocks to break
    double&         RR,             // Reduction ratio to apply to the rocks
    VectorView&     Sieves,         // Vector of sieve sizes
    VectorView&     Product         // Fractional size distribution to add
)                                   //   breakage products to
{
    // Convert RR into an Ecs
    double Ecs = CalcEcs( RR );

    // Convert Ecs into a T10
    double T10 = CalcT10( Ecs );

    // Break the rock into the Product[] size distribution
    BreakRockT10( RockSize, Quantity, T10, Sieves, Product );

    // Calculate energy required for breakage
    return Quantity * Ecs;
}




/****************************************************************************
*
*   MineralInfo1::BreakRockEcs( )
*
*   Break a quantity of rock into a size distribution of daughter particles
*   depending on the strength of a specific energy applied to the rock.
*
****************************************************************************/
double MineralInfo1::BreakRockEcs
(
    double&         RockSize,       // The size of the rocks to break
    double&         Quantity,       // The mass/volume of the rocks to break
    double&         Ecs,            // Specific energy to apply
    VectorView&     Sieves,         // Vector of sieve sizes
    VectorView&     Product         // Fractional size distribution to add
)                                   //   breakage products to
{
    // Convert Ecs into a T10
    double T10 = CalcT10( Ecs );

    // Break the rock into the Product[] size distribution
    BreakRockT10( RockSize, Quantity, T10, Sieves, Product );

    // Calculate energy required for breakage
    return Quantity * Ecs;
}




/****************************************************************************
*
*   MineralInfo1::BreakRockT10( )
*
*   Break a quantity of rock into a size distribution of daughter particles
*   depending on the value of T10 that is required of the product size
*   distribution.
*
****************************************************************************/
void MineralInfo1::BreakRockT10
(
    double&         RockSize,       // The size of the rocks to break
    double&         Quantity,       // The mass/volume of the rocks to break
    double&         T10,            // T10 required
    VectorView&     Sieves,         // Vector of sieve sizes
    VectorView&     Product         // Fractional size distribution to add
)                                   //   breakage products to
{
    // Use pre-calculated splines to get T curve for this T10

    PARAMT[0] = 100.0;
    PARAMT[1] = SPNV3( 4, X, Y2,  S2,  T10 );
    PARAMT[2] = SPNV3( 4, X, Y4,  S4,  T10 );
    PARAMT[3] = T10;
    PARAMT[4] = SPNV3( 4, X, Y25, S25, T10 );
    PARAMT[5] = SPNV3( 4, X, Y50, S50, T10 );
    PARAMT[6] = SPNV3( 4, X, Y75, S50, T10 );

    // Construct size x percent-passing coordinates

    YY[6] = log( PARAMT[0] );
    YY[5] = log( PARAMT[1] );
    YY[4] = log( PARAMT[2] );
    YY[3] = log( PARAMT[3] );
    YY[2] = log( PARAMT[4] );
    YY[1] = log( PARAMT[5] );
    YY[0] = log( PARAMT[6] );

    XX[6] = log( RockSize /  1.0 );
    XX[5] = log( RockSize /  2.0 );
    XX[4] = log( RockSize /  4.0 );
    XX[3] = log( RockSize / 10.0 );
    XX[2] = log( RockSize / 25.0 );
    XX[1] = log( RockSize / 50.0 );
    XX[0] = log( RockSize / 75.0 );

    // Calculate spline through size x percent passing

    SPN3( 7, XX, YY, SS );

    // Use spline to determine daughter product quantities

    const int N = Sieves.size();
    const int M = N-1;

    double prevPassing = 100.00;
    double scaleFactor = 0.01 * Quantity;

    for( int i=0; i<N; i++ )
    {
        double ThisSize = Sieves[i];
        double Passing  = 100.0;

        if( ThisSize >= RockSize )
        {
            Passing = 100.0;
        }
        else if( fabs(ThisSize)<1e-8 )
        {
            Passing = 0.0;
        }
        else
        {
            Passing = exp( SPNV3( 7, XX, YY, SS, log(ThisSize) ) );
            if( Passing>100.00 ) Passing = 100.00;
            if( Passing>prevPassing ) Passing = prevPassing;
        }

        Product[i] += scaleFactor * ( prevPassing - Passing );
        prevPassing = Passing;
    }
}



bool MineralInfo1::SetDefaultAppearance( )
{
    // Matrix is three rows, five columns
    appMatrix_.resize( 3, 5 );    

    // Reference appearance matrix rows
    VectorView R1 = appMatrix_[0];
    VectorView R2 = appMatrix_[1];
    VectorView R3 = appMatrix_[2];

    // Fill with default data
//    R1[0]=1.90; R1[1]= 2.70; R1[2]= 5.30; R1[3]=19.80; R1[4]=42.40;
//	R2[0]=4.50; R2[1]= 6.20; R2[2]=11.20; R2[3]=38.20; R2[4]=71.40;
//	R3[0]=7.90; R3[1]=10.50; R3[2]=17.50; R3[3]=54.80; R3[4]=89.40;

	R1[0]=2.00; R1[1]= 2.70; R1[2]= 4.70; R1[3]=27.90; R1[4]=55.90;
	R2[0]=5.20; R2[1]= 6.70; R2[2]=10.50; R2[3]=49.30; R2[4]=81.40;
	R3[0]=8.30; R3[1]=10.70; R3[2]=16.40; R3[3]=65.60; R3[4]=98.50;

	// Compute appearance splines
	AppearancePreCalc( );

	return true;
}



//-- MineralInfo1.cpp -------------------------------------------------------
