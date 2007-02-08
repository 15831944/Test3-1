// externals

#include "TSBallMill.h"
#include "CubicSpline2.h"

using namespace RioTintoTS;
//#pragma optimize("", off)

/****************************************************************************
*
*   BallMill::BallMill( )
*
*   Construct default BallMill object
*
****************************************************************************/
BallMill::BallMill( )
{
    // Create streams that we own
    
    Discharge = PFlowStream1( new FlowStream1 );
}


/****************************************************************************
*
*   BallMill::~BallMill( )
*
****************************************************************************/
BallMill::~BallMill( )
{
	/* void */
}

/*
'
' Evaluation of R/D* of Whiten Ball Mill model, given values at spline knots
'
'''
'''     Refer:
'''         Napier-Munn et al.
'''         "Mineral Comminution Circuits - Their Operation and Optimisation",
'''         JKMRC monograph 1996,
'''         pp 216
'''
'''
'''                Coded: Dave Wiseman September 2002
'''                Ported C++: Mark West May 2005
'''
'''
*/

void WhitenBallMill_R_on_Dstar(
							   const  Vector splineKnots,       // Knot size values
                               const  Vector splineRonDvalues,  // Knot RonD values
                               /*const*/  Vector sizes,		        // Size Intervals
							   Vector &R_on_Dstar)				// R_on_DStar @ sizes

{
	Vector X;
	Vector Y;
	CubicSpline2 Spline; 
	long nKnots = 0;
	// Count how many knots there really are
    for (int iKnot = 0 ; iKnot < splineKnots.size() ; iKnot++ )
	{
        if (splineKnots[iKnot] > 0.0 )
            nKnots = nKnots + 1;
		else
            break;
	}
    
	X.resize(nKnots);
	Y.resize(nKnots);

    for (int iKnot = 0 ; iKnot < nKnots; iKnot++)
	{
        X[iKnot] = log(splineKnots[iKnot]);
        Y[iKnot] = splineRonDvalues[iKnot];
	}

	Spline.SetSpline(X,Y);

	long nSizes = sizes.size();
	R_on_Dstar.resize(nSizes);


    for ( int iSize = 0; iSize < nSizes; iSize++ )
	{
		// TSModelLib uses mm as the size unit
		// Initialisation code should have set knot sizes to have unit of mm as well
		// from SI units (i.e. m)
		R_on_Dstar[iSize] = exp(Spline.CalculateY(log(sizes[iSize])));
	}

/* Testing */
/*
R_on_Dstar[0] = 14.86;R_on_Dstar[1] = 18.64;R_on_Dstar[2] = 23.39;R_on_Dstar[3] = 29.34;R_on_Dstar[4] = 36.81;R_on_Dstar[5] = 46.19;R_on_Dstar[6] = 57.94;R_on_Dstar[7] = 72.07;R_on_Dstar[8] = 87.19;R_on_Dstar[9] = 100.6;R_on_Dstar[10] = 108.4;R_on_Dstar[11] = 107.0;R_on_Dstar[12] = 94.74;R_on_Dstar[13] = 74.03;R_on_Dstar[14] = 51.39;R_on_Dstar[15] = 32.32;R_on_Dstar[16] = 18.80;R_on_Dstar[17] = 10.31;R_on_Dstar[18] = 5.445;R_on_Dstar[19] = 2.823;*/
}

/*
'
' Model of a BallMill - Simplified!!
'                       1. Assumes root 2 series as input
'                       2. Given a full array of R/D* values
'                       3. No JK Scaling (can be done on worksheet if desired!!)
'
'   Model originally developed by Bill Whiten, JKMRC
'
'''     Refer:
'''         Napier-Munn et al.
'''         "Mineral Comminution Circuits - Their Operation and Optimisation",
'''         JKMRC monograph 1996,
'''         pp 214-215
'''             NB. Equations 9.7 & 9.8 in the above text have incorrect
'''                 subscripts in the particle breakage summations.
'''                 Refer to figure 9.4 for the correct subscripts.
'''
'''
'''                Coded: Dave Wiseman September 2002
'''                Ported C++: Mark West May 2005
'''
'''
*/

void     WhitenBallMill(double nMills,
                        const VectorView millFeed,
                        double millFeedVolume,
                        double millDiameter,
                        double millLength,
                        const VectorView appearanceFunction,
                        const VectorView R_on_Dstar,
						VectorView millDischarge)
{
    long nSizes;
    long i;
    long j;
    long k;
    double Ri_on_Di;
    double Pi;
    double subMesh;
    Vector Pj_by_Rj_on_Dj;
    double RonDstar_to_RonD;

	nSizes = millFeed.size();
    Pj_by_Rj_on_Dj.resize(nSizes);

    RonDstar_to_RonD = 4 * (millFeedVolume / nMills) / (millDiameter * millDiameter * millLength);

    subMesh = millFeed.sum();

#define Mark 1
#ifdef Mark
	
    // This is my version which I think is easier to understand
	// Both give the same result.

    for (i = 0; i < (nSizes - 1); i++ )
	{

        Ri_on_Di = R_on_Dstar[i] / RonDstar_to_RonD;
 
        Pi = 0.0;
		double temp = 0.0;
        for (j = 0; j <= i; j++ )
		{
			temp += appearanceFunction[i-j] * R_on_Dstar[j] / RonDstar_to_RonD * millDischarge[j];
		}
        Pi = (millFeed[i]+temp) / (Ri_on_Di + 1.0);

        millDischarge[i] = Pi;

        subMesh = subMesh - Pi;         // submesh by difference

	}

#else

	// This is the translated LIM code except I think the index
	// into the appearance array was out by 1 when we use 0 based indexing

    for (i = 0; i < (nSizes - 1); i++ )
	{

        Ri_on_Di = R_on_Dstar[i] / RonDstar_to_RonD;
        Pi = 0.0;

        for (j = 0; j <= i; j++ )
		{
            k = i - j; // index to allow upside down array of Pj_by_Rj_on_Dj,
                       // saving need to shuffle array down each step
            //Pi = Pi + (appearanceFunction[j + 1] * Pj_by_Rj_on_Dj[k]);
			Pi = Pi + (appearanceFunction[j] * Pj_by_Rj_on_Dj[k]); // Testing
		}
        Pi = (Pi + millFeed[i]) / (Ri_on_Di + 1.0);

        Pj_by_Rj_on_Dj[i] = Pi * Ri_on_Di;

        millDischarge[i] = Pi;

        subMesh = subMesh - Pi;         // submesh by difference

	}
#endif
    millDischarge[nSizes-1] = subMesh;

}

/****************************************************************************
*
*   BallMill::Intialize( )
*
*   Configure BallMill model.
*
****************************************************************************/

bool BallMill::Initialize
( 
 PStreamInfo1      Config,
 const VectorView&  ParamVec
 )
{
	// Test object arguments

	if( Config==0 ) goto initFailed;

	// Test size of parameter vector

	if( ParamVec.size() < 108 ) goto initFailed;

	// Set parameters
	config_		= Config;

	// Retrieve stream model dimensions
	nSize_ = config_->nSize();
	nType_ = config_->nType();

	// Test that pBalluct stream is valid
	if( Discharge==0 )
		goto initFailed;

	// Configure Discharge stream
	if( !Discharge->SetConfig(config_) )
		goto initFailed;

	// Resize internal vectors

	C_.resize( nSize_ );
	nomSize_.resize( nSize_ );

	ModelOutput.resize( 80 );
	mi_KnotSize.resize( nSize_ );
	mi_KnotLnRDStar.resize( nSize_ );	
	mi_A.resize( nSize_ );
	mo_KnotLnRDStar.resize( nSize_ );
	mo_RDStar.resize( nSize_ );

	mi_NParrallelMills =			ParamVec[0];
	mi_DiameterSim =				ParamVec[1];
	mi_LengthSim =					ParamVec[2];
	mi_FracCriticalSpeedSim =		ParamVec[3];
	mi_FracLoadSim =				ParamVec[4];
	mi_WorkIndexSim =				ParamVec[5];
	mi_BallTopSizeSim =				ParamVec[6]*1000; // Convert si to mm
	mi_DiameterDerived =			ParamVec[7];
	mi_LengthDerived =				ParamVec[8];
	mi_FracCriticalSpeedDerived =	ParamVec[9];
	mi_FracLoadDerived =			ParamVec[10];
	mi_WorkIndexDerived =			ParamVec[11];
	mi_BallTopSizeDerived =			ParamVec[12]*1000; // Convert si to mm
	mi_K =						    ParamVec[13];
	mi_NumSplineKnots =				ParamVec[14];
	for ( int i = 0 ; i < 31 ; i++ )
		if (i < nSize_ ) mi_KnotSize[i] = ParamVec[15+i]*1000; // convert si to mm
	for ( int i = 0 ; i < 31 ; i++ )
		if (i < nSize_ ) mi_KnotLnRDStar[i] = ParamVec[46+i];
	for ( int i = 0 ; i < 31 ; i++ )
		if (i < nSize_ ) mi_A[i] = ParamVec[77+i];

	return true;

initFailed:

	return false;
}



/****************************************************************************
*
*   BallMill::CalculateModel( )
*
****************************************************************************/
bool BallMill::CalculateModel( PFlowStream1 FeedStream )
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

		FeedStream->Refresh();
        MatrixView& FEED  = FeedStream->AccessSolidsMatrix();
        MatrixView& PBall  = Discharge->AccessSolidsMatrix();
        VectorView& SIZE  = config_->GetSizes();

        //-- Construct nominal size vector ----------------------------------

        // coarsest fraction
        nomSize_[0] = sqrt(2.0f) * SIZE[0];

        // undersize fraction
        nomSize_[nSize_-1] = 0.5 * SIZE[nSize_-2];

        // intermediate fractions
        for( i=1; i<(nSize_-1); i++ )
            nomSize_[i] = sqrt( SIZE[i] * SIZE[i-1] );

        //-- Transport water to Discharge streams -----------------------------

        Discharge->SetLiquidMass( FeedStream->GetLiquidMass() );

		//-- Calculate Scaling Factors
		mo_FACTA = sqrt(mi_DiameterSim/mi_DiameterDerived);
		mo_FACTB = (1.0 - mi_FracLoadSim)*mi_FracLoadSim/((1.0 - mi_FracLoadDerived)*mi_FracLoadDerived);
		mo_FACTC = mi_FracCriticalSpeedSim/mi_FracCriticalSpeedDerived;
		mo_FACTD = pow( mi_WorkIndexSim/mi_WorkIndexDerived , 0.8 );
		mo_FACTEsizeltXm = (mi_BallTopSizeDerived/mi_BallTopSizeSim);
		mo_FACTEsizegeXm = pow((mi_BallTopSizeSim/mi_BallTopSizeDerived),2.0);

		//-- Scale the Knots
		mo_Xm = mi_K*mi_BallTopSizeSim*mi_BallTopSizeSim;
		for ( int i = 0 ; i < mi_NumSplineKnots ; i++ )
		{
			if (mi_KnotSize[i] < mo_Xm )
				mo_KnotLnRDStar[i] = mi_KnotLnRDStar[i]*mo_FACTA*mo_FACTB*mo_FACTC*mo_FACTC*mo_FACTD*mo_FACTEsizeltXm;
			else
				mo_KnotLnRDStar[i] = mi_KnotLnRDStar[i]*mo_FACTA*mo_FACTB*mo_FACTC*mo_FACTC*mo_FACTD*mo_FACTEsizegeXm;
		}

		//-- Calculate RonD*
	    //WhitenBallMill_R_on_Dstar(mi_KnotSize,       // Knot size values
        //                          mi_KnotLnRDStar,   // Knot RonD values
        //                          nomSize_,		     // Nominal Size Intervals
		//					      mo_RDStar);			
	    WhitenBallMill_R_on_Dstar(mi_KnotSize,       // Knot size values
                                  mo_KnotLnRDStar,   // Knot RonD values
                                  nomSize_,		     // Nominal Size Intervals
							      mo_RDStar);			

		//-- Calculate the total feed volume ???
		//double millFeedVolume = 412.1;
		double millFeedVolume = FeedStream->SolidsVolume() + FeedStream->GetLiquidMass();

        //-- Process each feed component to discharge stream ----------------

        for( i=0; i<nType_; i++ )
        {
            // Refer to columns of size distributions
            TableVector iFEED = FEED.column(i);
            TableVector iPBall = PBall.column(i);
            // Build materials in Discharge stream

			iPBall.clear();
			WhitenBallMill(mi_NParrallelMills,
                        iFEED,
                        millFeedVolume,
                        mi_DiameterSim,
                        mi_LengthSim,
                        mi_A,
                        mo_RDStar,
						iPBall);

        }
    }

    // Refresh data in streams
    Discharge->Refresh();

	// Construct output vector

	for ( int i = 0 ; i < 31 ; i++ )
		if ( i < nSize_) ModelOutput[i] = mo_RDStar[i];
	for ( int i = 0 ; i < 31 ; i++ )
		if ( i < nSize_) ModelOutput[31+i] = mo_KnotLnRDStar[i];
	ModelOutput[62] = mo_FACTA;
	ModelOutput[63] = mo_FACTB;
	ModelOutput[64] = mo_FACTC;
	ModelOutput[65] = mo_FACTD;
	ModelOutput[66] = mo_FACTEsizeltXm;
	ModelOutput[67] = mo_FACTEsizegeXm;
	ModelOutput[68] = mo_Xm;
	// Success
    return true;

calculateFail:

    // Failed to calculate model
    return false;
};






