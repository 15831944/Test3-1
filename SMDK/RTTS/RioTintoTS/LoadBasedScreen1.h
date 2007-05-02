#pragma once
#ifndef LOADBASEDSCREEN1__
#define LOADBASEDSCREEN1__
#include "TS.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"
#include "ObjectPtr.h"
#include "CubicSpline.h"
#include "Vector.h"
#include "Matrix.h"

namespace RioTintoTS
{

// Forward declarations.
class LoadBasedScreen1;

//! Smart pointer typedef for class LoadBasedScreen1
typedef ObjectPtr<LoadBasedScreen1> PLoadBasedScreen1;

/****************************************************************************
*
*   class LoadBasedScreen1
*
****************************************************************************/
class TS_API LoadBasedScreen1
{

public:

    // Build empty default LoadBasedScreen1 object

    LoadBasedScreen1( );

    // Release LoadBasedScreen1 resources

    ~LoadBasedScreen1( );

    // Initialize model with specific arguments

    bool LoadBasedScreen1::Initialize
    ( 
         PStreamInfo1  Config,
         VectorView&   ParamVec
    );

    virtual bool CalculateModel( PFlowStream1 FeedStream );

    PFlowStream1 Oversize;      // stream containing Oversize
    PFlowStream1 Undersize;     // stream containing Undersize

    Vector ModelOutput;         // vector of model calculated values
    Vector PartitionCurve;      // calculated partition curve

	double* pAArray;
	double* pBArray;

private:

	// Model Dimensions

    Vector Sizes;               // size distribution sieve series

    int    nSize;               // number of size fractions
    int    nType;               // number of mineral types

    // Model Parameters

	double Apperture;           // screen apperture
	double Length;			    // screen deck length
	double Width;				// screen deck width
	double Angle;               // screen deck inclination (not used)
	double OpenFraction;        // screen open area fraction
	double BulkDensity;         // feed material assumed bulk density
	int    WetScreening;		// 0-dry screening 1-wet screening
	int    AppertureShape;		// 
	int	   MediaType;
	int    DeckLocation;
	int	   GravelCorrection;
	double CustomAreaFactor;
	double WaterSplitToUS;

    double AF;                  // effective area
    double S;                   // screen opening
    double OA;                  // open fraction of effective area
    double F;                   // feed bulk density
    int    WS;                  // wet screen (boolen 1 = true, 0 = false)
    int    OT;                  // opening type (int)
    int    ST;                  // screen type
    int    DL;                  // deck location
    int    FT;                  // feed type
    double CF;                  // custom factor
    double WR;                  // water recovery of undersize
		
    Vector CombRetSizing;
    Vector CombOS;
    Vector CombUS;

    // Screen area factors

    double A;
    double B;
    double C;
    double D;
    double E;
    double G;
    double H;
    double J;
    double K;
    double L;
    double X;                   // Custom area factor


    // Model calculated values

    double Feed_OS;             // Fraction feed > apperture size
    double Feed_HS;             // Fraction feed < 1/2 apperture
    double Feed_US;             // Fraction feed < apperture size

    double D50;                 // Partition separation size

    double QF;                  // Total feed rate (tph)
    double QU;                  // Flow to undersize (tph)
    double T;                   // Screen efficiency (apperture size)
    double U;                   // Fraction feed < apperture size
    double V;                   // Screen loading % wrt design load

    // Area factor calculation routines

    double CalculateA( double S );
    double CalculateB( double FractOS );
    double CalculateC( double FractHS );
    double CalculateD( int DL );
    double CalculateE( int WS, double S );
    double CalculateG( double V, double T );
    double CalculateH( int OT );
    double CalculateJ( int ST, double S, double OA );
    double CalculateK( int FT );
    double CalculateL();
    double CalculateX( double CF );
    double CalculateT( double V );

    // Regression curve cubic splines - Capacity Factors

    CubicSpline ASpline;        // Apperture factor
    CubicSpline BSpline;        // Oversize in feed factor
    CubicSpline CSpline;        // Halfsize in feed fa factor
    CubicSpline ESpline;        // Wet-screening apperture factor
    CubicSpline JSpline1;       // Open area factor - Steel wire
    CubicSpline JSpline2;       // Open area factor - Steel plate
    CubicSpline JSpline3;       // Open area factor - Rubber
    CubicSpline JSpline4;       // Open area factor - polyurethane
    CubicSpline TSpline;        // Load v. Efficiency relation

    // Utility - partition single sizing into US/OS based on D50 and apperture size

    void CalculatePartition ( VectorView& Sizing, VectorView& Feed, double S, double d50,
                              VectorView& P, VectorView& OS, VectorView& US);

    // Calculate difference between predicted QU and partition QU

    double QUError( double d50 );

    // Solve for D50 that sets QUERRor() to zero

    double zbrent( double x1, double x2, double TOLERANCE);


};

} // namespace RioTintoTS

#endif