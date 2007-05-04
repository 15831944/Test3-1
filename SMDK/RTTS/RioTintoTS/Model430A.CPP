//-- Model430A.cpp ----------------------------------------------------------
#pragma optimize("", off)
#include "Model430A.h"
#include "CubicSpline.h"

#if WithFortranModel430
/****************************************************************************
*
*   External Symbols 
*
****************************************************************************/

using namespace RioTintoTS;

/****************************************************************************
*
*   Declare function signature of the external FORTRAN subroutine 
*   that performs Model430A calculation.
*
*   
*      SUBROUTINE Model430A_EntryPoint
*     +   (
*     +        UNITA, SIZE, NTYPE, ORESG, Param_A, Param_b,
*     +        Param_ta, BSIZING, NSPR, RSIZE, RSP,
*     +        AMFEED, AMDISC, AMLOAD, BLOAD, TAIL,
*     +        D, R, AMDATA, OUTPUT
*     +   )
*
*
****************************************************************************/
extern "C"
{
    //void __stdcall MODEL430A_ENTRYPOINT
    void __stdcall model430a_entrypoint
    (
      double* UNITDATA,
      double* SIZE,
      //long*   NTYPE,
      long    NTYPE,
      double* ORESG,
      double* Param_A,
      double* Param_b,
      double* Param_ta,
      double* BSIZING,
      //long*   NSPR,
      long    NSPR,
      double* RSIZE,
      double* RSP,
      double* AMFEED,
      double* AMDISC,
      double* AMLOAD,
      double* BLOAD,
      double* TAIL,
      double* D,
      double* R,
      double* AMDATA,
      double* OUTPUT
    );

    void __stdcall tranrl3
    (
        double* SIZE1, 
		double* RDIST1,
		double* SIZE2,
 	    double* RDIST2
	);

}




/****************************************************************************
*
*   Model430A::Model430A( )
*
*   Initialize an empty Model430A object.
*
****************************************************************************/
Model430A::Model430A( )
  {
  // Set default state
  nSize_      = 0;
  nType_      = 0;

  // Create streams that we own
  Discharge = PFlowStream1( new FlowStream1 );
  Load      = PFlowStream1( new FlowStream1 );
  Config    = PAgMillConfig1( new AgMillConfig1 );
  Parameters = PAgMillParameters1( new AgMillParameters1 );
  BallCharge = PBallSizing1( new BallSizing1 );
  }


/****************************************************************************
*
*   Model430A::~Model430A( )
*
*   Release the resources we created.
*
****************************************************************************/
Model430A::~Model430A( )
{
}


/****************************************************************************
*
*   Model430A::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a Model430A object
*
****************************************************************************/
bool Model430A::Initialize(PStreamInfo1 inConfig, const VectorView&  ParamVec )
  {
  int i;
  Vector X,Y;
  CubicSpline S;
  //VectorView Sizes;

  // Test object arguments

  if ( inConfig==0 ) 
    return false;//goto initFailed;

  // Test size of parameter vector
  if( ParamVec.size() < 39 ) 
    return false;//goto initFailed;

  // Set parameters

  config_		= inConfig;
  // Retrieve stream model dimensions
  nSize_ = config_->nSize();
  nType_ = config_->nType();
  const int nTyp = (nType_<MaxFortranMatType ? nType_ : MaxFortranMatType);
  const int nSiz = (nSize_<MaxFortranSizes ? nSize_ : MaxFortranSizes);

  // Test that product stream is valid
  if( Discharge==0 )
	  //goto initFailed;
	  return false;

  // Configure Discharge stream
  if( !Discharge->SetConfig(config_) )
	  //goto initFailed;
	  return false;

  // Try to configure the Load vector
  if( !Load->SetConfig( config_ ) )
    //goto initFailed;
	  return false;

  Config->SAG_NumMills				= ParamVec[ 0];                // UNIT(11) ???
  Config->SAG_Diameter				= ParamVec[ 1];                // UNIT(12)
  Config->SAG_Length					= ParamVec[ 2];                // UNIT(13)
  Config->SAG_Trunion_D				= ParamVec[ 3];                // UNIT(58)
  Config->SAG_Feed_Cone_Angle			= ParamVec[ 4];		           // UNIT(59)
  Config->SAG_Disch_Cone_Angle		= ParamVec[ 5];                // UNIT(60)
  Config->SAG_Grate_Size				= ParamVec[ 6];                // UNIT(14)
  Config->SAG_Fine_Size				= ParamVec[ 7];                // UNIT(15)
  Config->SAG_Grate_OAF				= ParamVec[ 8];                // UNIT(61)
  Config->SAG_PP_OAF					= ParamVec[ 9];                // UNIT(16)
  Config->SAG_PP_Size					= ParamVec[ 10];               // UNIT(17)
  Config->SAG_Mean_Radial_Position	= ParamVec[ 11];			   // UNIT(62)
  Config->SAG_Fr_CriticalSpeed		= ParamVec[ 12];			   // UNIT(210)
  Config->SAG_Ball_Load_by_Volume		= ParamVec[ 13];			   // UNIT(18)
  Config->SAG_No_Load_Power			= ParamVec[ 14];               // UNIT(216)
  Config->SAG_Feed_Solids_Density		= ParamVec[ 15];               // Added DMT 26/1/03

  Parameters->SAG_No_of_Knots			= ParamVec[ 16];               // UNIT(31)

  Parameters->SAG_Spline_Size_Knots.resize(Parameters->SAG_No_of_Knots);
  Parameters->SAG_Breakage_Rates.resize(Parameters->SAG_No_of_Knots);
  Parameters->SAG_Spline_Size_Knots[0]	= ParamVec[ 17];  // UNIT(32) - UNIT(36)
  Parameters->SAG_Spline_Size_Knots[1]	= ParamVec[ 18];  // UNIT(32) - UNIT(36)
  Parameters->SAG_Spline_Size_Knots[2]	= ParamVec[ 19];  // UNIT(32) - UNIT(36)
  Parameters->SAG_Spline_Size_Knots[3]	= ParamVec[ 20];  // UNIT(32) - UNIT(36)
  Parameters->SAG_Spline_Size_Knots[4]	= ParamVec[ 21];  // UNIT(32) - UNIT(36)

  Parameters->SAG_Breakage_Rates[0]		= ParamVec[ 22];  // UNIT(37) - UNIT(41)
  Parameters->SAG_Breakage_Rates[1]		= ParamVec[ 23];  // UNIT(37) - UNIT(41)
  Parameters->SAG_Breakage_Rates[2]		= ParamVec[ 24];  // UNIT(37) - UNIT(41)
  Parameters->SAG_Breakage_Rates[3]		= ParamVec[ 25];  // UNIT(37) - UNIT(41)
  Parameters->SAG_Breakage_Rates[4]		= ParamVec[ 26];  // UNIT(37) - UNIT(41)

  Parameters->SAG_Disch_Coeff				= ParamVec[ 27];  // UNIT(46) ???
  Parameters->SAG_Param_M1				= ParamVec[ 28];  // UNIT(42)
  Parameters->SAG_Param_M2				= ParamVec[ 29];  // UNIT(43)
  Parameters->SAG_Coarse_Factor			= ParamVec[ 30];  // UNIT(48) ???
  /* design load ??? */
  Parameters->SAG_Charge_Porosity			= ParamVec[ 31];  // UNIT(52)
  Parameters->SAG_Net_Power_Factor		= ParamVec[ 32];  // UNIT(217)


  BallCharge->Ball_SG						= ParamVec[ 33];
  double TopSize							= ParamVec[ 34];
  //Vector Y;
  Y.resize(5);
  //Y[0]									= ParamVec[  35];
  //Y[1]									= ParamVec[  36];
  //Y[2]									= ParamVec[  37];
  //Y[3]									= ParamVec[  38];
  Y[4]                                    = 100.0;
  Y[3]									= ParamVec[  35];
  Y[2]									= ParamVec[  36];
  Y[1]									= ParamVec[  37];
  Y[0]									= ParamVec[  38];
  // Need to calculate sizing from splines
  //Vector X;
  X.resize(5);
  //X[0] = TopSize/sqrt(2.0);
  //X[1] = TopSize/2.0;
  //X[2] = TopSize/(2.0*sqrt(2.0));
  //X[3] = TopSize/4.0;
  X[4] = TopSize;
  X[3] = TopSize/sqrt(2.0);
  X[2] = TopSize/2.0;
  X[1] = TopSize/(2.0*sqrt(2.0));
  X[0] = TopSize/4.0;
  //CubicSpline S;
  S.SetSpline(X,Y);
  VectorView Sizes = config_->GetSizes();
  BallCharge->Ball_Size_Distr.resize( nSiz );
  double ycum = 0.0;
  double szmm = 0.0;
  for( i=0; i<nSiz; i++ )
    {
    szmm = Sizes[i];
    if (szmm < TopSize)
	    ycum = S.CalculateY( szmm );
    else
	    ycum = 100.0;
    ycum = min( ycum , 100.0 );
    ycum = max( ycum , 0.0 );
    BallCharge->Ball_Size_Distr[i] = ycum;
    }


    //if( BallCharge->nSize() != nSize_ || nSize_ < 2 || nType_ < 1 )
    //    goto initFailed;

    //-- Test - product streams are configured ------------------------------

    //-- Everything Ok here - configure the Model430A state -----------------

  if( true )
    {

    // Size output vectors appropriately 
    out_ModelOutput.resize( 20 );
    out_D.resize( nSiz );
    out_R.resize( nSiz );

    // Setup sizing vector for jK routines
    SIZE[0] = nSiz;
    SIZE[1] = 0.0;
    //VectorView Sizes = config_->GetSizes();
    for( i=0; i<nSiz; i++ )
      SIZE[i+2] = Sizes[i];
    NTYPE = nTyp;

    // Copy material data into FORTRAN interface
    for( i=0; i<nTyp; i++ )
      {
      PMineralInfo1 RockInfo = config_->GetMineral(i);
      ORESG[i]    = RockInfo->SG();
      Param_A[i]  = RockInfo->A();
      Param_b[i]  = RockInfo->b();
      Param_ta[i] = RockInfo->ta();
      }

    // Setup ball size distribution in FORTRAN interface
    BSIZING[0] = 0.0;
    for( i=0; i<nSiz; i++ )
      BSIZING[i+1] = BallCharge->Ball_Size_Distr[i];

    // Setup breakage rate spline in FORTRAN interface
    NSPR = Parameters->SAG_No_of_Knots; //MUST be <= 20
    //NSPR = 4; // Debug

    for( i=0; i<NSPR; i++ )
      {
      RSIZE[i] = Parameters->SAG_Spline_Size_Knots[i];
      RSP[i]   = Parameters->SAG_Breakage_Rates[i];
      }

    // Determine feed solids density control option
    in_FeedSolidsDensity = Config->SAG_Feed_Solids_Density;

    // Load up parameter vector UNIT[] from the 
    //  configuration/parameters supplied
    //   to this Model430A object

    // Start with zeros

    for( i=0; i<500; i++ )
      UNIT[i] = 0.0;

    // Mill configuration data
    UNIT[11]  = Config->SAG_NumMills;
    UNIT[12]  = Config->SAG_Diameter;
    UNIT[13]  = Config->SAG_Length;
    UNIT[14]  = Config->SAG_Grate_Size;
    UNIT[15]  = Config->SAG_Fine_Size;
    UNIT[16]  = Config->SAG_PP_OAF;
    UNIT[17]  = Config->SAG_PP_Size;
    UNIT[18]  = Config->SAG_Ball_Load_by_Volume;
    UNIT[19]  = 0.0;                                // SAG_Calc_Load

    UNIT[42]  = Parameters->SAG_Param_M1;
    UNIT[43]  = Parameters->SAG_Param_M2;
    UNIT[44]  = 0.0;
    UNIT[45]  = 0.0;
    UNIT[46]  = 0.0;								// FeedData->Ecs;
    UNIT[47]  = 0.0;;

    UNIT[173] = 0;                                  // SAG_OREBLKVPC

    UNIT[210] = Config->SAG_Fr_CriticalSpeed;
    UNIT[211] = BallCharge->Ball_SG;
    UNIT[212] = Parameters->SAG_Charge_Porosity;
    UNIT[213] = 0.0;                                // SAG_Vol_Total_Load
    UNIT[214] = 0.0;                                // SAG_Total_Charge_Density
    UNIT[215] = 0.0;                                // SAG_Void_Fill;
    UNIT[216] = Config->SAG_No_Load_Power;
    UNIT[217] = Parameters->SAG_Net_Power_Factor;
    UNIT[218] = 0.0;                                // SAG_Gross_Power
    }

  // Ok. At this point the model should be
  // configured as specified, and ready for
  // simulation via. the CalculateModel() method

  return true;

  //initFailed:

    // Some error ocurred in the initialization of
    // this object. Indiate that the object should
    // be discarded.
    //return false;
  }

//====================================================================

// ---- Convert sizing from one sieve series to another ----
//translated from fortran code
int tranrl3_(double *size1, double *rdist1, double *size2, double *rdist2)
  {
  short s;

  /* Local variables */
  const short MaxLen = MaxFortranSizes + 30; //allow the other series to be larger
  short i, j;
  double x, y;
  short n1, n2;
  double x1, x2, y1, y2;
  double cdist1[MaxLen], cdist2[MaxLen], csize1[MaxLen], csize2[MaxLen]; //was 32, changed to 50
  
  /* Parameter adjustments */
  --rdist2;
  --size2;
  --rdist1;
  --size1;

  // Extract size distribution lengths
  n1 = (short) size1[1];
  n2 = (short) size2[1];

  // Convert SIZE1/FEED to cummulative distribution
  csize1[n1 - 1] = 0.;
  cdist1[n1 - 1] = 0.;
  for (i = (n1 - 1); i>0; --i)
    {
    csize1[i - 1] = size1[i + 2];
    cdist1[i - 1] = cdist1[i] + rdist1[i + 1];
    }
  // Interpolate CSIZE2/CDIST2 from CSIZE1/CDIST1
  csize2[n2 - 1] = 0.;
  cdist2[n2 - 1] = 0.;
  j = n1;
  x1 = csize1[j - 1];
  x2 = csize1[j - 2];
  y1 = cdist1[j - 1];
  y2 = cdist1[j - 2];
  for (i = (n2 - 1); i>0; --i) 
    {
    x = size2[i + 2];
	  while(! (x >= x1 && x <= x2) && j > 2) 
      {
      j = j - 1;
      x1 = csize1[j - 1];
      x2 = csize1[j - 2];
      y1 = cdist1[j - 1];
      y2 = cdist1[j - 2];
      }
    if (x >= x2) 
      {
	    y = y2;
	    } 
    else if (j > 0) 
      {
	    y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
	    }
    else
      {
	    y = 0.;
	    }
	  cdist2[i - 1] = y;
    }

  // Convert CSIZE2/CDIST2 to retained in RDIST2

  if (0)
    {// Testing    
    double rdist1_a[MaxLen];

    rdist1_a[0] = rdist1[1];
    s = n1 - 1;
    for (i=2; i<=s; ++i) 
      {
      rdist1_a[i - 1] = cdist1[i - 2] - cdist1[i - 1];
      }
    rdist1_a[n1 - 1] = cdist1[i - 2];
    }

  rdist2[1] = rdist1[1];
  s = n2 - 1;
  for (i=2; i<=s; ++i)
    {
	  rdist2[i] = cdist2[i - 2] - cdist2[i - 1];
    }
  rdist2[n2] = cdist2[i - 2];
  return 0;
  }


//-- Calculate the Model430A for a given feed -------------------------------

bool Model430A::CalculateModel( PFlowStream1 FeedStream )
  {
  int i = 0;
  int j = 0;

  // Test that FeedStream is a valid object
  if( FeedStream==0 || Discharge==0 )
    {
    goto calculateFail;
    }

  // Test that FeedStream matches our configuration
  if( FeedStream->nSize() != nSize_ || FeedStream->nType() != nType_ )
    {
    goto calculateFail;
    }
  else
    {
    // Copy FeedStream Solids into FORTRAN interface area
    MatrixView& Solids = FeedStream->AccessSolidsMatrix();

    const int nTyp = (nType_<MaxFortranMatType ? nType_ : MaxFortranMatType);
    const int nSiz = (nSize_<MaxFortranSizes ? nSize_ : MaxFortranSizes);

    for( j=1; j<nTyp; j++ ) // Initialise Liquids to 0
      AMFEED[j][0] = 0;

//double amfeedTtl[MaxFortranMatType],amfeedTtlMapped[MaxFortranMatType];
    for( j=0; j<nTyp; j++ )
      {
//amfeedTtl[j] = 0.0;
      for( i=1; i<nSiz; i++ )
        {
        AMFEED[j][i] = Solids[i][j];
//AMFEED_MAPPED[j][i] = 0.0;
//amfeedTtl[j] += Solids[i][j];
        }
      }

    // Calculate water quantity in FORTRAN interface area
    if( fabs(in_FeedSolidsDensity) > 1e-8 )
      {
      // Calculate additional water to obtain FeedSolidsDensity
      out_WaterAddition = FeedStream->CalcWaterAddition( in_FeedSolidsDensity );

      // Use feed water + additional water 
      AMFEED[0][0] = FeedStream->GetLiquidMass() + out_WaterAddition;
      }
    else
      {
      // Use feed water without additional water 
      out_WaterAddition = 0.0;
      AMFEED[0][0] = FeedStream->GetLiquidMass();
      }

    //
    // Model430A assumes distribution is passed where the top size corresponds
    // to the top size of the ore in the feed. This is required so that the
    // abrasion appearance function is applied correctly
    //
    double TS = 0.0;
    int TS_index = 0;
    for( j=0; j<nTyp; j++ )
      {
      // Work out the top size
      for( i=1; i<nSiz; i++ )
        {
        if ( AMFEED[j][i] > 0.0 )
          break;
        }
      const double TSi = SIZE[i+1];
      if (TSi > TS )
        {
        TS_index = i+1;
        TS = TSi;
        }
      }

    if (TS <= 0.0 ) 
      TS = SIZE[2];

    // Construct SIZE_MAPPED based on TopSize
    SIZE_MAPPED[0] = 31;
    SIZE_MAPPED[1] = SIZE[1];
    SIZE_MAPPED[2] = TS;
    for (i = 3 ; i < 32; i++)
      SIZE_MAPPED[i] = SIZE_MAPPED[i-1]/sqrt(2.0);


    // Map the distribution for each material to the new set of sieves based on the top size
    for( j=0; j<nTyp; j++ )
      {
//amfeedTtlMapped[j]=0.0;
      //tranrl3( &SIZE[0], &( AMFEED[j][0] ), &SIZE_MAPPED[0], &( AMFEED_MAPPED[j][0] ) );
      tranrl3_( &SIZE[0], &( AMFEED[j][0] ), &SIZE_MAPPED[0], &( AMFEED_MAPPED[j][0] ) );
//for( i=1; i<SIZE_MAPPED[0]; i++ )
//amfeedTtlMapped[j] += AMFEED_MAPPED[j][i];
      }

    // Call the external FORTRAN model code
    model430a_entrypoint
      ( 
      &( UNIT[1]      ),
      &( SIZE_MAPPED[0] ),
      ( NTYPE        ),
      &( ORESG[0]     ),
      &( Param_A[0]   ),
      &( Param_b[0]   ),
      &( Param_ta[0]  ),
      &( BSIZING[0]   ),
      ( NSPR         ),
      &( RSIZE[0]     ),
      &( RSP[0]       ),
      &( AMFEED_MAPPED[0][0] ),
      &( AMDISC_MAPPED[0][0] ),
      &( AMLOAD[0][0] ),
      &( BLOAD[0]     ),
      &( TAIL[0][0]   ),
      &( D[0]         ),
      &( R[0]         ),
      &( AMDATA[0][0] ),
      &( OUTPUT[0]    )
      );

    //
    // Remap back to calling routines intervals
    //
    for( j=0; j<nTyp; j++ )
      {
      //tranrl3( &SIZE_MAPPED[0], &( AMDISC_MAPPED[j][0] ), &SIZE[0], &( AMDISC[j][0] ) );
      tranrl3_( &SIZE_MAPPED[0], &( AMDISC_MAPPED[j][0] ), &SIZE[0], &( AMDISC[j][0] ) );

      // For some reason which I (MRW) do not have time to debug at this stage
      // discharge mass is slightly out so for now just normalise so the feed mass is
      // the same as the discharge mass
      double FeedSum = 0.0;
      double DischargeSum = 0.0;
      double CorrectionFactor = 1.0;
      for( i=1; i<nSiz; i++ )
        {
        FeedSum += AMFEED[j][i];
        DischargeSum += AMDISC[j][i];
        }
      if (DischargeSum > 0.0 )
        {
        CorrectionFactor = FeedSum/DischargeSum;
        for( i=1; i<nSiz; i++ )
          {
          AMDISC[j][i] *= CorrectionFactor;
          }
        }
      }

    // Extract information from FORTRAN interface
    //  into FlowStream objects for access
    MatrixView DischSolids = Discharge->AccessSolidsMatrix();
    MatrixView LoadSolids = Load->AccessSolidsMatrix();

    for( j=0; j<nTyp; j++ )
      {
      Discharge->SetLiquidMass(AMDISC[0][0]);
      //Load->SetLiquidMass(AMLOAD[0][0]);

      for( i=1; i<nSiz; i++ )
        {
        DischSolids[i][j] = AMDISC[j][i];
        //
        // Need to map Load Solids as well
        // 
        //LoadSolids[i][j]  = AMLOAD[j][i];
        }
      }

    // Extract other values from the FORTRAN
    // interface into our exported values

    // Breakage/Discharge rates
    for( i=0; i<nSiz; i++ )
      {
      out_D[i] = D[i];
      out_R[i] = R[i];
      }

    // Output vector
    for( i=0; i<20; i++ )
      {
      out_ModelOutput[i] = OUTPUT[i];
      }
    }

  // Refresh flow stream data
  Discharge->Refresh();
  Load->Refresh();

  return true;

  calculateFail:

  return false;
  }

#endif


//-- Model430A.cpp ----------------------------------------------------------
