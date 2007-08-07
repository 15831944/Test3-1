//-- LoadBasedScreen.cpp ----------------------------------------------------

#include "LoadBasedScreen1.h"

using namespace RioTintoTS;

          

/****************************************************************************
*
*   LoadBasedScreen1 Constants 
*
*   Un-named namepsace - prevent external linkage
*
****************************************************************************/

namespace
{
    const double TOLERANCE = 1E-7;

    const int ITMAX = 100;
    const double EPS = 3E-8;

    #define SIGN(a,b) ((b) >=0.0 ? fabs(a) : -fabs(a))

    const int    ARows = 19;

    const double ARegressionData[ARows][2] =
    {
    //    S         A
    //  Screen    Basic
    //  Opening  Capacity
        { 0    ,   0.0 },
        { 1    ,   1.1 },
        { 2    ,   2.0 },
        { 3    ,   3.0 },
        { 4    ,   3.9 },
        { 5    ,   4.8 },
        { 6.35 ,   5.9 },
        { 8    ,   7.0 },
        { 10   ,   8.2 },
        { 12   ,   9.4 },
        { 15   ,  10.7 },
        { 18   ,  12.0 },
        { 22   ,  13.4 },
        { 26   ,  14.6 },
        { 30   ,  15.8 },
        { 40   ,  18.2 },
        { 75   ,  25.0 },
        { 160  ,  41.5 },
        { 500  , 107.5 },
    };

    const double BRegressionData[ ][2] = 
    {
    //   Fract       B
    //  Oversize  Oversize
    //  in Feed    Factor
        { 0.00  ,  1.60 },
        { 0.50  ,  1.00 },
        { 0.80  ,  0.64 },
        { 0.85  ,  0.56 },
        { 0.90  ,  0.44 },
        { 0.95  ,  0.24 },
        { 1.00  ,  0.00 },
    };

    static double CRegressionData[ ][2] = 
    {
    //  Fract of     C
    // Feed < S/2  Factor
        { 0.00  ,  0.70 },
        { 0.25  ,  1.00 },
        { 0.30  ,  1.06 },
        { 0.35  ,  1.13 },
        { 0.40  ,  1.21 },
        { 0.45  ,  1.30 },
        { 0.50  ,  1.40 },
        { 0.55  ,  1.52 },
        { 0.60  ,  1.67 },
        { 0.65  ,  1.85 },
        { 0.70  ,  2.05 },
        { 0.75  ,  2.26 },
        { 0.80  ,  2.50 },
        { 0.85  ,  2.75 },
        { 0.90  ,  3.20 },
        { 0.95  ,  4.00 },
        { 0.98  ,  6.00 },
        { 1.00  , 10.00 },
    };

    const double ERegressionData[ ][2] =
    {
    //    S          E	
    // Screen  Wet Screening
    // Opening    Factor
        { 0    ,   1.0 },
        { 1    ,   1.2 },
        { 3    ,   2.0 },
        { 5    ,   2.0 },
        { 8    ,   1.6 },
        { 11   ,   1.4 },
        { 17   ,   1.2 },
        { 22   ,   1.1 },
        { 35   ,   1.0 },
        { 40   ,   1.0 },
    };

    const double JRegressionData[ ][5] =
    {
    //    S       1      2      3       4        <-- ST
    // Screen   Steel  Steel  Rubber Polyurethane
    // Opening  Wire   Plate
        {   1  , 0.40 , 0.40 , 0.20 , 0.20 },
        {   5  , 0.50 , 0.45 , 0.35 , 0.40 },
        {   8  , 0.60 , 0.45 , 0.35 , 0.40 },
        {  11  , 0.70 , 0.50 , 0.40 , 0.45 },
        {  15  , 0.70 , 0.55 , 0.40 , 0.45 },
        {  20  , 0.70 , 0.60 , 0.45 , 0.50 },
        {  40  , 0.70 , 0.60 , 0.50 , 0.50 },
        {  80  , 0.75 , 0.65 , 0.50 , 0.55 },
        { 160  , 0.75 , 0.65 , 0.50 , 0.55 },
    };

    const double TRegressionData[ ][2] = 
    {
    //      V        T
    //    Load   Efficiency
        {  000.0 , 0.80 },
        {  062.5 , 0.95 },
        {  075.0 , 0.95 },
        {  100.0 , 0.90 },
        {  450.0 , 0.50 },
        {  500.0 , 0.45 },
        {  562.5 , 0.40 },
        {  642.9 , 0.35 },
        {  750.0 , 0.30 },
        {  900.0 , 0.25 },
        { 1125.0 , 0.20 },
    };


}

/****************************************************************************
*
*   Initialize default, empty LoadBasedScreen1
*
****************************************************************************/
LoadBasedScreen1::LoadBasedScreen1( )
  {
  // Create new product stream objects 
  Undersize = PFlowStream1( new FlowStream1 );
  Oversize  = PFlowStream1( new FlowStream1 );

  pAArray = NULL;
  pBArray = NULL;
  pCArray = NULL;
  pEArray = NULL;
  pTdArray = NULL;
  pTwArray = NULL;
  }
   

/****************************************************************************
*
*   Release resources held by LoadBasedScreen1 object
*
****************************************************************************/
LoadBasedScreen1::~LoadBasedScreen1( )
  {
  pAArray = NULL;
  pBArray = NULL;
  pCArray = NULL;
  pEArray = NULL;
  pTdArray = NULL;
  pTwArray = NULL;
  }


/****************************************************************************
*
*   Construct LoadBasedScreen1 object as specified by specific arguments
*
*
*
*   ScreenIntialize( Sizing, nRockType, ParamVec )
*
*   Initialize the screen model local variables to match the screen
*   specified by:
*
*       Sizing    - Vector of sieve sizes coarsest to finest for
*                   the size distributions in the feed stream to the model
*   
*       nRockType - The number of mineral types in the feed stream (columns)
*
* THIS NEEDS UPDATING DAVE 13 params and different order
*       ParamVec  - Screen configuration vector (10 elements). Contains:
*
*         ParamVec[0] : Screen apperture in mm
*         ParamVec[1] : Screen total effective area m^2
*         ParamVec[2] : Open fraction of effective area
*         ParamVec[3] : Bulk density of the screen feed t/m^3
*         ParamVec[4] : Wet screening flag:
*                         0 - normal screening
*                         1 - wet screening
*         ParamVec[5] : Screen opening type
*                         1 - Round
*                         2 - Square
*                         3 - Slot 2/1
*                         4 - Slot 3/1
*                         5 - Slot 4/1
*                         6 - Slot 5/1
*                         7 - Grizzly
*         ParamVec[6] : Screen Material Type
*                         1 - Steel Wire
*                         2 - Steel Plate
*                         3 - Rubber
*                         4 - Polyurethane
*         ParamVec[7] : Deck location
*                         1 - Top Deck or Grizzly
*                         2 - Second Deck
*                             etc
*         ParamVec[8] : Feed Type (Gravel Factor)
*                         1 - Rock
*                         2 - Gravel
*         ParamVec[9] : Custom area factor = 1.0 for no change
*
*		  ParamVec[10]: Water Recovery of Undersize
*		  ParamVec[11]: Water Recovery of Undersize
*		  ParamVec[12]: Water Recovery of Undersize
*
****************************************************************************/
bool LoadBasedScreen1::Initialize( PStreamInfo1 Config, VectorView& ParamVec )
  {
  // get count of material types from MatInfo
  nType = Config->nType();

  // determine number of sizes from SizInfo
  nSize = Config->nSize();

  // determine number of paramaters
  int nParam = ParamVec.size();

  // ensure at least one type and two sizes
  if( nType<1 || nSize <2 )
    goto initFailed;

  // ensure parameter vector has 11 elements
  //if( nParam != 11 )
  //    goto initFailed;
  // ensure parameter vector has 13 elements
  if( nParam != 13 )
    goto initFailed;

  // set local vector/matrix to correct dimension
  Sizes.resize( nSize );
  PartitionCurve.resize( nSize );
  CombRetSizing.resize( nSize );
  PartitionCurve.resize( nSize );
  CombOS.resize( nSize );
  CombUS.resize( nSize );
  ModelOutput.resize( 23 );

  // set screen product streams to correct dimension
  Oversize->SetConfig( Config );
  Undersize->SetConfig( Config );

  // extract sieve sizes
  Sizes = Config->GetSizes();

  // unpack parameters from ParamVec

  Apperture		      = ParamVec[ 0];
  Length				    =	ParamVec[ 1];
  Width				      =	ParamVec[ 2];
  Angle				      =	ParamVec[ 3];
  OpenFraction		  =	ParamVec[ 4];
  BulkDensity			  =	ParamVec[ 5];
  WetScreening		  = (int) ParamVec[ 6];
  AppertureShape		= (int) ParamVec[ 7];
  MediaType			    = (int) ParamVec[ 8];
  DeckLocation		  = (int) ParamVec[ 9];
  GravelCorrection	= (int) ParamVec[10];
  CustomAreaFactor	=	ParamVec[11];	
  WaterSplitToUS		=	ParamVec[12];	

  S   = Apperture;
  AF  = Length * Width;
  OA  = OpenFraction;
  F   = BulkDensity;
  WS  = WetScreening;
  OT  = AppertureShape;
  ST  = MediaType;
  DL  = DeckLocation;
  FT  = GravelCorrection;
  CF  = CustomAreaFactor;
  WR  = WaterSplitToUS;

  // indicate success
  return true;

  initFailed:

  // initialization failed - set default values

  nSize = 0;
  nType = 0;
  // etc

  // indicate failure
  return false;
  }

void LoadBasedScreen1::SetWaterSplitToUS(double WSUS)
  {
  WaterSplitToUS = WSUS;
  WR  = WaterSplitToUS;
  }

void LoadBasedScreen1::SetApperture(double Ap)
  {
  Apperture = Ap;
  S = Apperture;
  }

void LoadBasedScreen1::InitializeRegr(double* A_, double* B_, double* C_, double* E_, double* Td_, double* Tw_)
  {
  pAArray = A_;
  pBArray = B_;
  pCArray = C_;
  pEArray = E_;
  pTdArray = Td_;
  pTwArray = Tw_;
  }

/****************************************************************************
*
*   LoadBasedScreen1::CalculateModel( FeedStream )
*
*   Calculate the product streams of an already initialzed screen
*   model when presented with the given feed stream matrix.
*
*   At the end of the function, the following variables
*   should contain updated values from the calculation:
*
*        ScreenOS           Matrix of oversize stream components
*        ScreenUS           Matrix of undersize stream components
*        PartitionCurve     Vector - contains screen partition curve
*        ModelOutput        Vector - assorted model calculated values:
*
*        ModelOutput[0]     QF       total tph of feed
*        ModelOutput[1]     Feed_OS  fraction of feed > apperture
*        ModelOutput[2]     Feed_US  fraction of feed < apperture
*        ModelOutput[3]     Feed_HS  fraction of feed < half-apperture
*        ModelOutput[4]     QU       flowrate of undersize stream
*        ModelOutput[5]     QO       flowrate of oversize stream
*        ModelOutput[6]     S        apperture size
*        ModelOutput[7]     T        screen efficiency (at S)
*        ModelOutput[8]     AF       effective screen area
*        ModelOutput[9]     D50      fitted d50 of separation
*        ModelOutput[10]    A        basic capacity factor
*        ModelOutput[11]    B        over-size factor
*        ModelOutput[12]    C        half-size factor
*        ModelOutput[13]    D        deck location factor
*        ModelOutput[14]    E        wet screening factor
*        ModelOutput[15]    F        feed bulk density factor
*        ModelOutput[16]    G        load vs efficiency factor
*        ModelOutput[17]    H        opening shape factor
*        ModelOutput[18]    J        screen open area factor
*        ModelOutput[19]    K        rock/gravel factor
*        ModelOutput[20]    L        humidity factor
*        ModelOutput[21]    X        user custom factor
*        ModelOutput[22]    V        screen load %
*
****************************************************************************/
//##ModelId=40A1898902EF
bool LoadBasedScreen1::CalculateModel( PFlowStream1 FeedStream )
  {
  // Local Variables
  int i = 0;
  int j = 0;
  double ScaleFactor = 0.00;
  double Denom = 0.00;

  Vector CombCPPSizing ( nSize );
  CubicSpline FeedSpline;    

  MatrixView FeedSolids      = FeedStream->AccessSolidsMatrix( );

  MatrixView OversizeSolids  = Oversize->AccessSolidsMatrix( );

  MatrixView UndersizeSolids = Undersize->AccessSolidsMatrix( );

  // Ensure correct shape feed matrix

  if( FeedSolids.rowCount() != nSize )
    goto calcFailed;

  if( FeedSolids.columnCount() != nType ) 
    goto calcFailed;

  // Calculate total feed flow rate (solids)

  QF = FeedSolids.sum( );

  // Convert feed matrix into single component size distribution

  for( i = 0; i < nSize; i++ )
    CombRetSizing[ i ] = (FeedSolids.row(i)).sum();

  if (fabs(QF)<1e-8)
    ScaleFactor = 0.00;
  else 
    ScaleFactor = 1/QF;

  // Scale single component distribution to cumulative sizing
  //  scaled to fraction basis: ie. 1 passes top-size

  CombCPPSizing[ nSize - 1 ] = 0;
  for( i = nSize - 2; i >=0; i-- )
    CombCPPSizing[ i ] = CombCPPSizing[i + 1] + ( CombRetSizing[i + 1] * ScaleFactor );

  // Construct cubic spline passing through cumulative curve    

  FeedSpline.SetSpline( Sizes, CombCPPSizing );

  // Use cubic spline to get fraction passing half apperture size

  Feed_HS = FeedSpline.CalculateY( S/2 );

  // Use spline to get fraction passing apperture size    

  U = Feed_US = FeedSpline.CalculateY( S );
  Feed_OS = 1 - Feed_US;

  // Calculate area factors for this screen

  A = CalculateA( S );
  B = CalculateB( Feed_OS );
  C = CalculateC( Feed_HS );
  D = CalculateD( DL );
  E = CalculateE( WS, S );
  H = CalculateH( OT );
  J = CalculateJ( ST, S, OA );
  K = CalculateK( FT );
  L = CalculateL();
  X = CalculateX( CF );

  // Calculate denominator of load equation - ensure not zero

  Denom = A * B * C * D * E * F * H * J * K * L * AF * X;
  if (fabs(Denom)<1e-8)
    Denom = 1e-8;

  // Calculate load

  V = ( 90 * QF * U ) / ( Denom );

  // Calculate efficiency that matches this load

  T = CalculateT( V );

  // Calculate factor G

  G = ( V * T ) / 90;

  // calculate flow to undersize at this efficiency

  QU = QF * U * T;

  // Solve for D50 that matches the undersize flow

  D50 = zbrent( S * 0.1, S * 0.99, TOLERANCE );

  // Calculate splitting of feed water to products

  Undersize->SetLiquidMass( FeedStream->GetLiquidMass() * WR );
  Oversize->SetLiquidMass( FeedStream->GetLiquidMass() * (1-WR) );

  // Calculate splitting of solids components to products 

  for( i=0; i<nType; i++ )
    {
    for( j=0; j<nSize; j++ )
      {
      double feed = FeedSolids[j][i];

      OversizeSolids[j][i] = feed * PartitionCurve[j];
      UndersizeSolids[j][i] = feed * ( 1 - PartitionCurve[j] );
      }
   }

  // Setup model output vector

  ModelOutput[0]  = QF;
  ModelOutput[1]  = Feed_OS;
  ModelOutput[2]  = Feed_US;
  ModelOutput[3]  = Feed_HS;
  ModelOutput[4]  = QU;
  ModelOutput[5]  = QF - QU;
  ModelOutput[6]  = S;
  ModelOutput[7]  = T;
  ModelOutput[8]  = AF;
  ModelOutput[9]  = D50;
  ModelOutput[10] = A;
  ModelOutput[11] = B;
  ModelOutput[12] = C;
  ModelOutput[13] = D;
  ModelOutput[14] = E;
  ModelOutput[15] = F;
  ModelOutput[16] = G;
  ModelOutput[17] = H;
  ModelOutput[18] = J;
  ModelOutput[19] = K;
  ModelOutput[20] = L;
  ModelOutput[21] = X;
  ModelOutput[22] = V;

  // Indicate success
  return true;

  calcFailed:

  // Indicate failure
  return false;
  }


//##ModelId=40A1898A0368
double LoadBasedScreen1::QUError( double d50 )
  {
  CalculatePartition( Sizes, CombRetSizing, S, d50, PartitionCurve, CombOS, CombUS );
  return CombUS.sum() - QU;
  }

//##ModelId=40A1898A037C
double LoadBasedScreen1::zbrent( double x1, double x2, double TOLERANCE)
  {
  int iter;
  double a=x1,b=x2,c=x2,d,e,min1,min2;
  double fa=QUError(a),fb=QUError(b),fc,p,q,r,s,tol1,xm;

  if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))
    {
    // error
    }

  fc=fb;
  for (iter=1;iter<=ITMAX;iter++) 
    {
    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) 
      {
	    c=a;
	    fc=fa;
	    e=d=b-a;
      }
    if (fabs(fc) < fabs(fb)) 
      {
	    a=b;
	    b=c;
	    c=a;
	    fa=fb;
	    fb=fc;
	    fc=fa;
      }
    tol1=2.0*EPS*fabs(b)+0.5*TOLERANCE;
    xm=0.5*(c-b);
    if (fabs(xm) <= tol1 || fb == 0.0) return b;
    if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) 
      {
	    s=fb/fa;
	    if (a == c) 
        {
		    p=2.0*xm*s;
		    q=1.0-s;
	      }
      else 
        {
		    q=fa/fc;
		    r=fb/fc;
		    p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
		    q=(q-1.0)*(r-1.0)*(s-1.0);
	      }
	    if (p > 0.0) 
        q = -q;
	    p=fabs(p);
	    min1=3.0*xm*q-fabs(tol1*q);
	    min2=fabs(e*q);
	    if (2.0*p < (min1 < min2 ? min1 : min2)) 
        {
		    e=d;
		    d=p/q;
	      }
      else 
        {
		    d=xm;
		    e=d;
	      }
      }
    else 
      {
	    d=xm;
	    e=d;
      }
    a=b;
    fa=fb;
    if (fabs(d) > tol1)
	    b += d;
    else
	    b += SIGN(tol1,xm);
    fb=QUError(b);
    }
  //	only arrive here if max. iteration is exceeded;
  return 0.0;
  }


/********************************
*								*
*	SCREEN BASIC CAPACITY (A)	*
*								*
*********************************/

// Calculate Paramater A for LoadBasedScreen1
//##ModelId=40A1898A017D
double LoadBasedScreen1::CalculateA( double S )
  {
  double A = 0;

  const int nRowsA = 19;
  static double AArray [ nRowsA ] [ 2 ] = 
	{
//          S       A	
//      Screen    Basic
//      Opening Capacity
        { 0    ,   0.0 },
		{ 1    ,   1.1 },
		{ 2    ,   2.0 },
		{ 3    ,   3.0 },
		{ 4    ,   3.9 },
		{ 5    ,   4.8 },
		{ 6.35 ,   5.9 },
		{ 8    ,   7.0 },
		{ 10   ,   8.2 },
		{ 12   ,   9.4 },
		{ 15   ,  10.7 },
		{ 18   ,  12.0 },
		{ 22   ,  13.4 },
		{ 26   ,  14.6 },
		{ 30   ,  15.8 },
		{ 40   ,  18.2 },
		{ 75   ,  25.0 },
		{ 160  ,  41.5 },
		{ 500  , 107.5 }, };
        
  double* p = (pAArray ? pAArray : *AArray);
  Matrix AMatrix( nRowsA, 2, p );
  CubicSpline ASpline;

	ASpline.SetSpline( AMatrix.column(0), AMatrix.column(1) );

	A = ASpline.CalculateY ( S );
	
	return A;
  }

/********************************
*								*
*	SCREEN OVERSIZE FACTOR (B)	*
*								*
*********************************/

// Calculate Paramater B for LoadBasedScreen1
//##ModelId=40A1898A0191
double LoadBasedScreen1::CalculateB( double FractOS )
  {	
	double B = 0;

	const int nRowsB = 7;
	static double BArray [ nRowsB ] [ 2 ] = 
	{
//       Fract       B	
//      Oversize  Oversize
//      in Feed    Factor
        { 0.00  ,  1.60 },
		{ 0.50  ,  1.00 },
		{ 0.80  ,  0.64 },
		{ 0.85  ,  0.56 },
		{ 0.90  ,  0.44 },
		{ 0.95  ,  0.24 },
		{ 1.00  ,  0.00 }, };

  double* p = (pBArray ? pBArray : *BArray);
	Matrix BMatrix( nRowsB, 2, p );
	CubicSpline BSpline;

	BSpline.SetSpline ( BMatrix.column(0), BMatrix.column(1) );

	B = BSpline.CalculateY ( FractOS );
    
//	Vector TempSplineVec( nRows );

//	SPN3( nRows, BMatrix.column(0), BMatrix.column(1), TempSplineVec );

//	B = SPNV3( nRows, BMatrix.column(0), BMatrix.column(1), TempSplineVec, FractOS );

	return B;
  }


/********************************
*								*
*	SCREEN HALFSIZE FACTOR (C)	*
*								*
*********************************/

// Calculate Paramater C for LoadBasedScreen1
//##ModelId=40A1898A019B
double LoadBasedScreen1::CalculateC( double FractHS )
  {
  double C = 0;

  const int nRowsC = 18;
  static double CArray [ nRowsC ] [ 2 ] = 
    {
//      Fract of    C	
//     Feed < S/2
//                Factor
        { 0.00 ,  0.70 },
        { 0.25 ,  1.00 },
        { 0.30 ,  1.06 },
        { 0.35 ,  1.13 },
        { 0.40 ,  1.21 },
        { 0.45 ,  1.30 },
        { 0.50 ,  1.40 },
        { 0.55 ,  1.52 },
        { 0.60 ,  1.67 },
        { 0.65 ,  1.85 },
        { 0.70 ,  2.05 },
        { 0.75 ,  2.26 },
        { 0.80 ,  2.50 },
        { 0.85 ,  2.75 },
        { 0.90 ,  3.20 },
        { 0.95 ,  4.00 },
        { 0.98 ,  6.00 },
        { 1.00 , 10.00 }, };

  double* p = (pCArray ? pCArray : *CArray);
	Matrix CMatrix( nRowsC, 2, p );
	CubicSpline CSpline;

	CSpline.SetSpline ( CMatrix.column(0), CMatrix.column(1) );

	C = CSpline.CalculateY ( FractHS );
    
//    Vector TempSplineVec( nRows );

//	SPN3( nRows, CMatrix.column(0), CMatrix.column(1), TempSplineVec );

//	C = SPNV3( nRows, CMatrix.column(0), CMatrix.column(1), TempSplineVec, FractHS );

	return C;
  }


/************************************
*									*
*	SCREEN DECK LOCATION FACTOR (D)	*
*									*
************************************/

// Calculate Paramater D for LoadBasedScreen1
//##ModelId=40A1898A01AF
double LoadBasedScreen1::CalculateD( int DL )
  {
	double D = 0;
                          // DECK LOCATION
  if  ( DL == 1 )         // Top Deck & Grizzly
    D = 1.0;
  else if ( DL == 2 )     // Second Deck
    D = 0.9;
  else if ( DL == 3 )     // Third Deck
  	D = 0.8;
  else if ( DL == 4 )     // Fourth Deck
  	D = 0.7;
  else if ( DL == 5 )     // Grizzly
    D = 1.0;
  else
    D = 1.0;			// Set default as Top Deck (1.0)
    
	return D;
  }


/********************************
*								*
*	WET SCREENING FACTOR (E)	*
*								*
*********************************/

// Calculate Paramater E for LoadBasedScreen1
//##ModelId=40A1898A01B9
double LoadBasedScreen1::CalculateE( int WS, double S )
  {
	double E = 0;

	const int nRowsE = 10;
	static double EArray [ nRowsE ] [ 2 ] = 
	{
//        S          E	
//     Screen  Wet Screening
//     Opening    Factor
#ifdef OLD
        { 0    ,   1.0 },
		{ 1    ,   1.5 },
		{ 3    ,   2.0 },
		{ 5    ,   2.0 },
		{ 8    ,   1.6 },
		{ 11   ,   1.4 },
		{ 17   ,   1.2 },
		{ 22   ,   1.1 },
		{ 35   ,   1.0 },
		{ 40   ,   1.0 }, };
#endif
        { 0    ,   2.88 },
		{ 1    ,   3.0 },
		{ 3    ,   3.0 },
		{ 5    ,   2.95 },
		{ 8    ,   2.93 },
		{ 11   ,   2.88 },
		{ 17   ,   2.73 },
		{ 22   ,   2.63 },
		{ 35   ,   1.88 },
		{ 40   ,   1.00 }, };

  if ( WS == 0)
    {
    E = 1.0;
    }
  else
    {
    double* p = (pEArray ? pEArray : *EArray);
    Matrix EMatrix( nRowsE, 2, p );
    CubicSpline ESpline;

    ESpline.SetSpline ( EMatrix.column(0), EMatrix.column(1) );

    E = ESpline.CalculateY ( S );
    }

	return E;	
  }


/****************************************
*										*
*	SCREEN LOAD & EFFICIENCY FACTOR (G)	*
*										*
****************************************/

// Calculate Paramater G for LoadBasedScreen1
//##ModelId=40A1898A01CE
double LoadBasedScreen1::CalculateG( double V, double T )
  {
	double G = 0;
	G = ( V * T ) / 90;
	return G;
  }


/********************************
*								*
*	SHAPE OF OPENING (H)		*
*								*
*********************************/

// Calculate Paramater H for LoadBasedScreen1
//##ModelId=40A1898A01EB
double LoadBasedScreen1::CalculateH( int OT )
  {
	double H = 0;
                               // SHAPE OF OPENING
  if  ( OT == 1 )            // Round Opening
    H = 0.80;
  else if ( OT == 2 )        // Square Opening
	  H = 1.00;
  else if ( OT == 3 )        // Slot 2/1 Opening
	  H = 1.05;
  else if ( OT == 4 )        // Slot 3/1 Opening
	  H = 1.10;
  else if ( OT == 5 )        // Slot 4/1 Opening
    H = 1.15;
  else if ( OT == 6 )        // Slot 5/1 Opening
    H = 1.20;
  else if ( OT == 7 )        // Grizzly
    H = 1.25;
  else
    H = 1.00;              // default Square
  return H;
  }


/********************************
*								*
*	SCREEN OPEN AREA FACTOR (J)	*
*								*
*********************************/

// Calculate Paramater J for LoadBasedScreen1
//##ModelId=40A1898A01F6
double LoadBasedScreen1::CalculateJ( int ST, double S, double OA )
  {
	double J = 0;
  double TOA = 0;                   // Table open area

  if ( ST < 1 )
    ST = 4;                       // default is Polyurethane
  if (ST > 4)
    ST = 4;                       // default is Polyurethane

	const int nRowsJ = 9;
	static double JArray [ nRowsJ ] [ 5 ] = 
	{
//        S       1  	 2      3       4        <-- ST
//     Screen   Steel  Steel  Rubber Polyurethane
//     Opening  Wire   Plate
        {   1  , 0.40 , 0.40 , 0.20 , 0.20 },
        {   5  , 0.50 , 0.45 , 0.35 , 0.40 },
        {   8  , 0.60 , 0.45 , 0.35 , 0.40 },
        {  11  , 0.70 , 0.50 , 0.40 , 0.45 },
        {  15  , 0.70 , 0.55 , 0.40 , 0.45 },
        {  20  , 0.70 , 0.60 , 0.45 , 0.50 },
        {  40  , 0.70 , 0.60 , 0.50 , 0.50 },
        {  80  , 0.75 , 0.65 , 0.50 , 0.55 },
        { 160  , 0.75 , 0.65 , 0.50 , 0.55 }, };

  Matrix JMatrix( nRowsJ, 5, *JArray );
  CubicSpline JSpline;

  JSpline.SetSpline ( JMatrix.column(0), JMatrix.column(ST) );

  TOA = JSpline.CalculateY ( S );


  J = OA/TOA;

	return J;
  }


/********************************
*								*
*	ROCK/GRAVEL FACTOR (K)		*
*								*
*********************************/

// Calculate Paramater K for LoadBasedScreen1
//##ModelId=40A1898A0213
double LoadBasedScreen1::CalculateK( int FT )
  {
	double K = 0;
                              // FEED MATERIAL
  if  ( FT == 1 )           // Rock
    K = 1.00; 
  else if ( FT == 2 )       // Gravel
	  K = 1.25;
  else
    K = 1.00;             // default Rock
  return K;
  }


/********************************
*								*
*	HUMIDITY FACTOR (L)			*
*								*
*********************************/

// Calculate Paramater L for LoadBasedScreen1
//##ModelId=40A1898A021E
double LoadBasedScreen1::CalculateL()
  {
	double L = 0;
  L = 1.00;             // This will decrease if feed includes clay, soil or recycling material
                        //  or the feed humidity is >3% and screen opening is <25 mm.
  return L;
  }


/********************************
*								*
*	CUSTOM USERS FACTOR (X)		*
*								*
*********************************/

// Calculate Paramater X for LoadBasedScreen1
//##ModelId=40A1898A0227
double LoadBasedScreen1::CalculateX( double CF )
  {
  double X = 0;

  if ( fabs (CF) < 1E-8 )
    X = 1.00;
  else 
    X = CF;

  return X;   
  }


/********************************
*								*
*	SCREEN EFFICIENCY (T)		*
*								*
*********************************/

// Calculate Paramater T for LoadBasedScreen1
//##ModelId=40A1898A0232
double LoadBasedScreen1::CalculateT( double V )
  {
  double T = 0;

  // Dry Screening Efficiency Curve
  // Light loading sees drop off in efficiency
  // Why? DT to expand
  // Where do these numbers come from ? DT to expand
  const int nDryRows = 11;
  static double TDryArray [ nDryRows ] [ 2 ] = 
  {
  //          V        T	
  //        Load   Efficiency
      {  000.0 , 0.90 },
      {  062.5 , 0.95 },
      {  075.0 , 0.95 },
      {  100.0 , 0.90 },
      {  450.0 , 0.50 },
      {  500.0 , 0.45 },
      {  562.5 , 0.40 },
      {  642.9 , 0.35 },
      {  750.0 , 0.30 },
      {  900.0 , 0.25 },
      { 1125.0 , 0.20 }, };

  const int nWetRows = 22;

  // Wet Screening Efficiency Curve
  // Light loading sees 100% efficiency
  // Why DT to expand
  // Where do these numbers come from ? DT to expand
  static double TWetArray [ nWetRows ] [ 2 ] = 

  {
  //          V        T	
  //        Load   Efficiency
  /*	{ 10	, 	.9917317  },
  { 20	, 	.9927046  },
  { 30	, 	.9910998  },
  { 39.3	, 	.9871216  },
  { 50	, 	.9776228  },
  { 60.5	, 	.963927	  },
  { 66.3	, 	.9553783  },
  { 75	, 	.9416755  },
  { 90	, 	.9167897  },
  { 100	 ,	.9001193  },
  { 110	 ,	.8838962  },
  { 120	 ,	.8684178  },
  { 130	 ,	.853857	  },
  { 138.9	 ,	.8417114  },
  { 144.1	 ,	.8349535  },
  { 150.9	 ,	.8264498  },
  { 157	 ,	.8190873  },
  { 169	 ,	.8050404  },
  { 184.5	 ,	.7866662  },
  { 200	 ,	.7659365  },
  { 210	 ,	.7499636  }};*/

  { 10	, 	.997  },
  { 20	, 	.996  },
  { 30	, 	.99   },
  { 39.3	, 	.982  },
  { 50	, 	.98   },
  { 63	, 	.96	  },
  { 75	, 	.95   },
  { 80	, 	.942  },
  { 90	, 	.923  },
  { 100	 ,	.90   },
  { 110	 ,	.887  },
  { 120	 ,	.873  },
  { 130	 ,	.862  },
  { 140	 ,	.849  },
  { 150	 ,	.837  },
  { 160	 ,	.827  },
  { 170	 ,	.813  },
  { 179.2	 ,	.801  },
  { 190	 ,	.785  },
  { 200	 ,	.77   },
  { 210	 ,	.755  },
  { 300    ,  .6    }};

  double* pw = (pTwArray ? pTwArray : *TWetArray);
  double* pd = (pTdArray ? pTdArray : *TDryArray);
  Matrix TWetMatrix( nWetRows, 2, pw );
  Matrix TDryMatrix( nWetRows, 2, pd );
  CubicSpline TSpline;

  if (WetScreening==true)
    TSpline.SetSpline ( TWetMatrix.column(0), TWetMatrix.column(1) );
  else
    TSpline.SetSpline ( TDryMatrix.column(0), TDryMatrix.column(1) );

  T = TSpline.CalculateY ( V );

  return T;
  }


/********************************
*								*
*	SCREEN OUTPUT				*
*								*
*********************************/

// Calculate Partition Curve for LoadBasedScreen1
//##ModelId=40A1898A02D1
void LoadBasedScreen1::CalculatePartition( VectorView& Sizing, VectorView& Feed, double S, double d50,
							 VectorView& P, VectorView& OS, VectorView& US )

{
	
	int nSize = Sizing.size();

    int i ( 0 );

    Vector MeanSize ( nSize );

    MeanSize[ 0 ] = Sizing[ 0 ] * sqrt(2.0f);

    for( i = 1; i < nSize - 1; i++ )
        MeanSize[ i ] = (sqrt(Sizing[i] * Sizing[i - 1]));
    
    MeanSize[ nSize -1 ] = (Sizing[ nSize - 1] + Sizing[ nSize - 2])/2;


    double a1 = -9.8120701;
    double a2 = 77.344372;
    double a3 = -244.80282;
    double a4 = 406.60363;
    double a5 = -389.07671;
    double a6 = 221.84784;
    double a7 = -73.704716;
    double a8 = 13.044678;
    double a9 = -0.9442089;

    if(fabs(S-d50)<1e-8)
        d50 = S - 1e-8;

    for( i = 0; i < nSize; i++ )
    {
        double d = MeanSize[ i ];
        double x = 1 + 0.4 * ( d - d50 ) / ( S - d50 );
        
        double x2 = x*x;
        double x3 = x2*x;
        double x4 = x3*x;
        double x5 = x4*x;
        double x6 = x5*x;
        double x7 = x6*x;
        double x8 = x7*x;

        if ( d <= S )
            P[ i] = a1*x8+a2*x7+a3*x6+a4*x5+a5*x4+a6*x3+a7*x2+a8*x+a9;
        else
            P[ i ] = 1;

        if (P[ i ] < 0)
            P[ i ] = 0;
        
        OS[ i ] = Feed[ i ] * P[ i ];

        US[ i ] = Feed[ i ] - OS[ i ];

    }
}

//-- LoadBasedScreen.cpp ----------------------------------------------------
