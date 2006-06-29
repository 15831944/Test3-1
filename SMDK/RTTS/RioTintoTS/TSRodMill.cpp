// externals

#include "TSRodMill.h"
#pragma optimize("", off)

using namespace RioTintoTS;

/****************************************************************************
*
*   RodMill::RodMill( )
*
*   Construct default RodMill object
*
****************************************************************************/
RodMill::RodMill( )
{
    // Create streams that we own
    
    Discharge = PFlowStream1( new FlowStream1 );
}

/****************************************************************************
*
*   RodMill::~RodMill( )
*
****************************************************************************/
RodMill::~RodMill( )
{
	/* void */
}


/****************************************************************************
*
*   RodMill::Intialize( )
*
*   Configure RodMill model.
*
****************************************************************************/
bool RodMill::Initialize
( 
 PStreamInfo1      Config,
 const VectorView&  ParamVec
 )
{
	// Test object arguments

	if( Config==0 ) goto initFailed;

	// Test size of parameter vector

	if( ParamVec.size() < 16 ) goto initFailed;

	// Set parameters

	config_		= Config;

	//MILL OPERATING VARIABLES
	mi_NParrallelMills =			ParamVec[ 0];
	mi_DiameterSim =				ParamVec[ 1];
	mi_LengthSim =					ParamVec[ 2];
	mi_FracCriticalSpeedSim =		ParamVec[ 3];
	mi_FracLoadSim =				ParamVec[ 4];
	mi_WorkIndexSim =				ParamVec[ 5];

	// MODEL PARAMETERS
	mi_DiameterDerived =			ParamVec[ 6];
	mi_LengthDerived =				ParamVec[ 7];
	mi_FracCriticalSpeedDerived =	ParamVec[ 8];
	mi_FracLoadDerived =			ParamVec[ 9];
	mi_WorkIndexDerived =			ParamVec[ 10];
	mi_MillConstantDerived =		ParamVec[ 11];
	mi_XC =							ParamVec[ 12];
	mi_IN =							ParamVec[ 13];
	mi_SL =							ParamVec[ 14];
	mi_F90Derived =					ParamVec[ 15];

	//  PARAMETERS 17-47 = ROD MILL APPEARANCE FUNCTION
	// How are we going to enter this or will it be calculated based on ore type??
	mi_A.resize( 31 );
	for (int i = 0; i < 31 ; i++)
		mi_A[i] = ParamVec[16+i];

	//  PARAMETERS 48-78 = ROD MILL CLASSIFICATION FUNCTION
	// How will we specify this or will it be calculated???
	// Doco talks about default classification values being same as sieves??
	mi_C.resize( 31 );
	for (int i = 0; i < 31 ; i++)
		mi_C[i] = ParamVec[47+i];


	// Retrieve stream model dimensions
	nSize_ = config_->nSize();
	nType_ = config_->nType();

	// Test that product stream is valid
	if( Discharge==0 )
		goto initFailed;

	// Configure Discharge stream
	if( !Discharge->SetConfig(config_) )
		goto initFailed;

	// Resize internal vectors

	C_.resize( nSize_ );
	T10.resize( nType_ );
	nomSize_.resize( nSize_ );

	Content_.resize( nSize_ );
	BreakFeed_.resize( nSize_ );
	BreakProd_.resize( nSize_ );

	mo_S.resize( nSize_ );
	ModelOutput.resize( 40 );

	return true;

initFailed:

	return false;
}

/****************************************************************************/
// Calculate the fraction that passes a particular size
//
// TO DO: Make this a stream function - DT already probably does have this
//
/****************************************************************************/

double PSFR3( VectorView& Size, MatrixView& FEED, double Percent )

{
Vector CSIZE1;
Vector CDIST1;

      // Extract size distribution lengths
	  // Smallest fraction is N1-1
      // Largest  fraction is 0
	  int N1 = Size.size();

      // Convert SIZE1/FEED to cummulative distribution
	  CSIZE1.resize(N1);
	  CDIST1.resize(N1);
      CSIZE1[N1-1] = 0;
      CDIST1[N1-1] = 0;

	  double ltotal = 0.0;
	  for (int i=(N1-2);i>=0;i--)
	  {
          //CSIZE1[i] = Size[i+1];
		  CSIZE1[i] = Size[i];
		  Vector v;
		  v = FEED.row(i+1);
		  double lsum = v.sum();
		  ltotal += lsum;
          CDIST1[i] = CDIST1[i+1] + lsum;
	  }

	  CDIST1 *= 1.0/(ltotal + .1e-30);

      //Search cumulative distribution
      //  for values that brackets X

      int J = N1;
      double X1 = CDIST1[J-1];
      double X2 = CDIST1[J-2];
      double Y1 = CSIZE1[J-1];
      double Y2 = CSIZE1[J-2];

      while ((((Percent >= X1) && (Percent <= X2) ) && (J > 1) )!=true)
	  {
          J = J-1;
          X1 = CDIST1[J];
          X2 = CDIST1[J-1];
          Y1 = CSIZE1[J];
          Y2 = CSIZE1[J-1];
	  }

      //Use linear interpolation to 
      // determine size Y that matches Percent

	  double Y = 0.0;
      if ( Percent >= X2 )
          Y = Y2;
	  else if ( J > 0 )
          Y = Y1 + (Y2-Y1)*(Percent-X1)/(X2-X1);
	  else
          Y = 0.0;

	return(Y);
}

/****************************************************************************/
// ---- Calculate the flow of material in a size distribution -------------
/****************************************************************************/
double MASSFLOW( VectorView& SIZE, VectorView& FEED )
{
      double flow = 0.0;
      int N;

      // Count size fractions
      N = SIZE.size();

      // Add flows of each size fraction
      for (int i = 0; i < N ; i++ )
          flow += FEED[i];

      // Return accumulated flow
      return(flow);
}

double MASSFLOW( VectorView& SIZE, double* FEED )
{
      double flow = 0.0;
      int N;

      // Count size fractions
      N = SIZE.size();

      // Add flows of each size fraction
      for (int i = 0; i < N ; i++ )
          flow += FEED[i];

      // Return accumulated flow
      return(flow);
}

/****************************************************************************/
// SIMULATE A ROD MILL - Translated from GSIM fortran source code
/****************************************************************************/

void   RM3( VectorView xSize ,
		    double RMC, double DELN,
			double NBreakStages,
		    VectorView B ,
		    VectorView C ,
		    VectorView S , 
		    VectorView F ,
		    VectorView P )


{
double    FEED[33];
double    PRODUCT[33];
double    TEMP[33];
double    fsaved[33];
double    B1;
double    FQ;
double    SUM;
double    V;
double    V1;
int		  I;
int		  IV;
int		  J;
int		  J1;
int		  N;
int		  N1;
int		  NN;
int		  NV;

	  FQ = MASSFLOW(xSize, F);
	  if (FQ <= 0.0) return;

      N1 = xSize.size();
      //N  = N1 - 1; // GSIM Returned size is 1 size larger as array includes size as well
	                 // We do not need to do this
	  N = N1;

	  //
	  // Shift input feed vector F by 1 as GSIM
	  // internal format has water in first array position
	  //
	  FEED[0] = 0.0;
	  PRODUCT[0] = 0.0;
	  for (int i = 0 ; i < N1 ; i++ )
	  {
		  FEED[i+1] = F[i];
		  PRODUCT[i+1] = 0.0;
	  }

	  V = NBreakStages;
      NV = int(V) + 1;
      if ( NV > N ) NV = N - 1;

	  // MOVE FEED TO WORKING ARRAY
      for (I = 1; I <= N ; I++)
	  {
         TEMP[I-1] = FEED[I-1];
		 fsaved[I-1] = 0.0;
	  }

      B1 = B[0] - 1.0;

	  // CALCULATE BREAKAGE FOR INTEGERS NEXT TO V
	  for ( IV = 1 ; IV <= NV ; IV++ )
	  {
         NN = N - IV;

		// SAVE SIZING OF FEED TO STAGE
        for (I = 0; I <= NN; I++)
              fsaved[I+IV-1] = TEMP[I+IV-1];

		// ONE STAGE OF BREAKAGE

         for (J = 1; J <= NN; J++ )
		 {
            J1 = J - 1;
            SUM = ( S[J-1] * B1 + 1.0 ) * fsaved[J+IV-1];
            if (J1 > 0 )
			{
               for (I = 1; I <= J1; I++)
                 SUM = SUM + B[J+1-I-1] * S[I-1] *
				      ( C[I-1] * TEMP[I+IV-1] + fsaved[I+IV-1] );


			}

            TEMP[J+IV-1] = SUM / (1.0 - (B1 * S[J-1] + 1.0) * C[J-1]+.1E-30);
	     }

		 // NOW THE PRODUCT FROM THE STAGE
         for (I = 1; I <= NN; I++)
            TEMP[I+IV-1] = (1.0 - C[I-1]) * TEMP[I+IV-1];

	  }

	  // CALCULATE FRACTIONAL STAGES OF BREAKAGE
      V1 = double(NV) - V;
      V  = 1.0 - V1;

      for (I = 2; I <= N1; I++)
         PRODUCT[I-1] = V1 * fsaved[I-1] + V * TEMP[I-1];

	  // SUBMESH - What does SUBMESH mean? Put the rest of the solids at size 0
      PRODUCT[N1] = 0.0;
      PRODUCT[N1] = FQ - MASSFLOW(xSize, &PRODUCT[1]);

	  // This routine assumes the first element has water
	  // Translate to our format (solids only)
	  for (int i = 0 ; i < N1 ; i++ )
	  {
		P[i] = PRODUCT[i+1];
	  }

}

/****************************************************************************
*
*   RodMill::CalculateModel( )
*
****************************************************************************/
bool RodMill::CalculateModel( PFlowStream1 FeedStream )
{
    // Test that streams are valid for use
    if( FeedStream==0 || Discharge==0 )
    {
        goto calculateFail;
    }
    else
    {
        //-- Setting Up -----------------------------------------------------

        int    i = 0;
        int    j = 0;

        double Error      = 0;

        double TOLERANCE  = 1e-8;
        int    ITERMAX    = 100;

        int    Iteration  = 0;

        MatrixView& FEED  = FeedStream->AccessSolidsMatrix();
        MatrixView& PROD  = Discharge->AccessSolidsMatrix();
        VectorView& SIZE  = config_->GetSizes();

		double TPH = FEED.sum();
		double F80 = FeedStream->CombinedP80();

        //-- Construct nominal size vector ----------------------------------

        // coarsest fraction
        nomSize_[0] = sqrt(2.0f) * SIZE[0];

        // undersize fraction
        nomSize_[nSize_-1] = 0.5 * SIZE[nSize_-2];

        // intermediate fractions
        for( i=1; i<(nSize_-1); i++ )
            nomSize_[i] = sqrt( SIZE[i] * SIZE[i-1] );

        //-- Transport water to product streams -----------------------------

        Discharge->SetLiquidMass( FeedStream->GetLiquidMass() );


		// CHANGE MILL CONSTANT IN PROPORTION TO RELATIVE MILL POWER CHANGE.
		//  MILL SIZE FACTOR ...

		double FACT = pow( mi_DiameterSim / (mi_DiameterDerived + .1E-30) , 2.5 )*
						+( mi_LengthSim   / (mi_LengthDerived + .1E-30));


		// COMBINED WITH LOAD FRACTION FACTOR ...

		double VOLF =  (1. - mi_FracLoadSim) * mi_FracLoadSim;
		double POLF  = (1. - mi_FracLoadDerived) * mi_FracLoadDerived;
		FACT = FACT * VOLF / (POLF+.1E-30);

		// COMBINED WITH FRACTION CRITICAL SPEED FACTOR ...
		FACT = FACT * mi_FracCriticalSpeedSim / (mi_FracCriticalSpeedDerived + .1E-30);


		// GIVES TOTAL MILL CONSTANT FACTOR TO MODIFY MILL CONSTANT
		mo_MCSim = mi_MillConstantDerived * FACT;


		// SELECTION FUNCTION
		//  CHANGE IN R/D DUE TO A WORK INDEX CHANGE
		double DELRD = -0.8 * log (mi_WorkIndexSim / (mi_WorkIndexDerived + .1E-30));

		//  CYCLE THROUGH SIZE FRACTIONS
		//		Coarset is index 0
        for( i=0; i<(nSize_); i++ )
		{
			// MEAN SIZE OF FRACTION 
			// Calculated previously
			//nomSize_[i]

			//  UNMODIFIED SELECTION FUNCTION IN INTERVAL
			double mi_XCmm = mi_XC*1000.0;
			double mi_INmm = mi_IN*1000.0;
			mo_S[i] = mi_SL * nomSize_[i] + mi_INmm;
			if (nomSize_[i] < mi_XCmm) 
				mo_S[i] = mi_SL * mi_XCmm  + mi_INmm;

			// CHECK FOR SELECTION FUNCTION OUT OF RANGE.
            if (( mo_S[i] >= 1.0) ||
		        ( mo_S[i] <= 0.0))
			{
				mo_S[i] = min(mo_S[i],1.0);
				mo_S[i] = max(mo_S[i],0.0);
			}
			else
			{

				// LOG R/D FROM SELECTION FUNCTION, SHIFTED BY WORK INDEX CHANGE
				double RDL = log( mo_S[i] / (1. - mo_S[i])) + DELRD;

				//  RECALCULATE NEW SELECTION, FROM SHIFTED R/D
				double RDE = exp(RDL);
				mo_S[i] = RDE / (1. + RDE+.1E-30);

				// Range 0-1
				mo_S[i] = min(mo_S[i],1.0);
				mo_S[i] = max(mo_S[i],0.0);

				//if (i==4)
				//	mo_S[i] = 0.322; //Debugging

			}

		}

		//  SHIFT NO. OF BREAKAGE STAGES IN PROPORTION TO NO. OF SIZE
		//  FRACTIONS (SQRT (2)) SHIFT IN MILL FEED 90% PASSING SIZE.
		//  FEED 90% PASSING SIZE
		mo_F90Sim = PSFR3( SIZE, FEED, 0.9 )/1000; // m


		//  CHANGE IN NO. OF BREAKAGE STAGES WITH FEED COARSENESS
        mo_Out_ChangeNStages = log( mi_F90Derived / (mo_F90Sim +0.1E-30)) / .34657;

		//  Calculate NO. OF BREAKAGE STAGES

		//FQ = MASSFLOW(xSize, FEED);
		double FQ = FEED.sum();

        //NUMBER OF STAGES OF BREAKAGE
	    mo_NBreakageStagesSim = pow(( mo_MCSim  / (FQ +.1E-30)) , .66666667) + mo_Out_ChangeNStages;

        //-- Process each feed component to discharge stream ----------------

        for( i=0; i<nType_; i++ )
		//for (i=0; i<1; i++)
        {
            // Refer to columns of size distributions
            TableVector iFEED = FEED.column(i);
            TableVector iPROD = PROD.column(i);

			//  CALL ROD MILL MODEL CALCULATION
			RM3(SIZE, mo_MCSim, mo_Out_ChangeNStages,mo_NBreakageStagesSim,
				      mi_A, mi_C, mo_S, iFEED, iPROD);

        }
    }

    // Refresh data in streams
    Discharge->Refresh();
	//P80 = Discharge->CombinedP80();

	// Construct output vector
	//  MODEL CALCULATED VALUES
	ModelOutput[ 0] = mo_MCSim;
	ModelOutput[ 1] = mo_F90Sim;
	ModelOutput[ 2] = mo_Out_ChangeNStages;
	ModelOutput[ 3] = mo_NBreakageStagesSim;
	//  PARAMETERS 83-113 = ROD MILL SELECTION FUNCTION
	for (int i = 0; i < 31 ; i++ )
		ModelOutput[4+i] = mo_S[i];
	//  PARAMETERS 114-144 = Sizes at which above function data occurred

	// Success
    return true;

calculateFail:

    // Failed to calculate model
    return false;
};






