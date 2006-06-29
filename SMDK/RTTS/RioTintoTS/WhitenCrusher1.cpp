// externals
#pragma optimize("", off)

#include "WhitenCrusher1.h"

using namespace RioTintoTS;


/****************************************************************************
*
*   WhitenCrusher1::WhitenCrusher1( )
*
*   Construct default WhitenCrusher1 object
*
****************************************************************************/
WhitenCrusher1::WhitenCrusher1( )
{
    // Create streams that we own
    
    Discharge = PFlowStream1( new FlowStream1 );
}




/****************************************************************************
*
*   WhitenCrusher1::~WhitenCrusher1( )
*
****************************************************************************/
WhitenCrusher1::~WhitenCrusher1( )
{
	/* void */
}




/****************************************************************************
*
*   WhitenCrusher1::Intialize( )
*
*   Configure WhitenCrusher1 model.
*
****************************************************************************/
bool WhitenCrusher1::Initialize
( 
 PStreamInfo1      Config,
 const VectorView&  ParamVec
 )
{
	// Test object arguments

	if( Config==0 ) goto initFailed;

	// Test size of parameter vector

	if( ParamVec.size() < 26 ) goto initFailed;

	// Set parameters

	config_		= Config;

	CSS		    = ParamVec[ 0];
	LLen	    = ParamVec[ 1];
	ET		    = ParamVec[ 2];
	LHr		    = ParamVec[ 3];
	MotorPower  = ParamVec[ 4];
	NoLoadPower = ParamVec[ 5];
	a0			= ParamVec[ 6];
	a1			= ParamVec[ 7];
	a2			= ParamVec[ 8];
	a3			= ParamVec[ 9];	   
	a4			= ParamVec[10];
	b0			= ParamVec[11];
	b1			= ParamVec[12];
	b2			= ParamVec[13];
	b3			= ParamVec[14];
	b4			= ParamVec[15];
	b5			= ParamVec[16];
	c0			= ParamVec[17];
	d0			= ParamVec[18];
	d1			= ParamVec[19];
	d2			= ParamVec[20];
	e0			= ParamVec[21];
	e1			= ParamVec[22];
	f0			= ParamVec[23];
	f1			= ParamVec[24];
	f2			= ParamVec[25];

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

	ModelOutput.resize( 11 );

	return true;

initFailed:

	return false;
}



/****************************************************************************
*
*   WhitenCrusher1::CalculateModel( )
*
****************************************************************************/
bool WhitenCrusher1::CalculateModel( PFlowStream1 FeedStream )
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

		TPH = FEED.sum();
		F80 = FeedStream->CombinedP80();

		k1 = a0*CSS + a1*TPH + a2*F80 + a3*LLen + a4;
		k2 = b0*CSS + b1*TPH + b2*F80 + b3*LHr  + b4*ET + b5;
		k3 = c0;

		Ecs = d0 * exp( -d1*CSS );  
		
		Capacity = e0*CSS + e1;

		NetPower = 0;

        //-- Construct nominal size vector ----------------------------------

        // coarsest fraction
        nomSize_[0] = sqrt(2.0f) * SIZE[0];

        // undersize fraction
        nomSize_[nSize_-1] = 0.5 * SIZE[nSize_-2];

        // intermediate fractions
        for( i=1; i<(nSize_-1); i++ )
            nomSize_[i] = sqrt( SIZE[i] * SIZE[i-1] );

        //-- Construct classification function C ----------------------------

        for( i=0; i<nSize_; i++ )
        {
            if( nomSize_[i] < k1 )
                C_[i] = 0;

            else if( nomSize_[i] < k2 )
                C_[i] = 1.0 - pow( (k2 - nomSize_[i])/(k2 - k1 ) , k3 );

            else
                C_[i] = 1.0;
        }

        //-- Transport water to product streams -----------------------------

        Discharge->SetLiquidMass( FeedStream->GetLiquidMass() );

        //-- Process each feed component to discharge stream ----------------

        for( i=0; i<nType_; i++ )
        {
            // Refer to columns of size distributions
            TableVector iFEED = FEED.column(i);
            TableVector iPROD = PROD.column(i);

            // Refer to material being processed
            PMineralInfo1 RockInfo = config_->GetMineral(i);

            // Calculate the T10 that will apply
            T10[i] = RockInfo->CalcT10( Ecs );

            //** Iteration Loop **//

            Content_.clear( );
            Iteration = 0;
            Error     = 2 * TOLERANCE;

            Content_  = iFEED;

            while( Error > TOLERANCE && Iteration < ITERMAX )
            {
                // Select content to feed breakage mechanism

                BreakFeed_ = Content_;
                BreakFeed_ *= C_;

                // Break each size fraction of selected content
                //  into breakage products

                BreakProd_.clear( );
                for( j=0; j<nSize_; j++ )
                {
					NetPower += BreakFeed_[j] * Ecs;

                    RockInfo->BreakRockT10
                    (
                        nomSize_[j],       // size of rock in fraction j
                        BreakFeed_[j],     // quantity of rock to break
                        T10[i],            // T10 of breakage product
                        SIZE,              // sizes in product distribution
                        BreakProd_         // vector to acquire fragments
                    );
                }

                // Copy breakage products + feed into content
                //   making note of maximum mis-convergence

                Error = 0;
                for( j=0; j<nSize_; j++ )
                {
                    double newC = iFEED[j] + BreakProd_[j];
                    double newE = abs( newC - Content_[j] );

                    if( newE > Error )
                        Error = newE;

                    Content_[j] = newC;
                }
            }

            // Build materials in Discharge stream
            iPROD  = Content_;
            iPROD -= BreakFeed_;
        }
    }

    // Refresh data in streams
    Discharge->Refresh();
	P80 = Discharge->CombinedP80();

	// Construct output vector

	GrossPower = NoLoadPower + NetPower;
	Utilization = ( fabs(Capacity)>1e-8 ? TPH / Capacity * 100 : 0 );

	ModelOutput[ 0] = TPH;
	ModelOutput[ 1] = Capacity;
	ModelOutput[ 2] = Utilization;
	ModelOutput[ 3] = F80;
	ModelOutput[ 4] = P80;
	ModelOutput[ 5] = k1;
	ModelOutput[ 6] = k2;
	ModelOutput[ 7] = k3;
	ModelOutput[ 8] = Ecs;
	ModelOutput[ 9] = NetPower;
	ModelOutput[10] = GrossPower;

	// Success
    return true;

calculateFail:

    // Failed to calculate model
    return false;
};






